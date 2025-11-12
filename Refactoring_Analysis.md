# 서버 아키텍처 리팩토링 분석 문서

## 1\. 초기 아키텍처의 문제점

본 리팩토링 이전의 서버는 다음과 같은 구조적 문제점들을 가지고 있었습니다.

1. **비기능적 게임 루프:** `Room::UpdateGame()` 함수는 게임의 핵심 로직(입력 처리, 상태 업데이트, 결과 전송)을 포함해야 하지만, 실제로는 어떤 스레드에서도 호출되지 않아 게임이 전혀 진행되지 않는 치명적인 버그가 있었습니다.
2. **불완전한 로직:** `Room::UpdateGame()` 내부의 `while(true)` 루프는 큐가 비어있을 때를 고려하지 않아 크래시 위험이 있었고, 고정 시간 스텝(Fixed Timestep) 개념이 없어 일관적인 게임 시뮬레이션이 불가능했습니다.
3. **스레드 안전성 부재:** `Room`의 `Players` 벡터와 `incomingQueue`는 여러 스레드(메인 스레드, 세션 스레드, 룸 스레드)에서 동시에 접근할 수 있었지만, 하나의 `CRITICAL\_SECTION`으로 불완전하게 보호되거나 전혀 보호되지 않아 `Race Condition`(경쟁 상태)에 매우 취약했습니다.
4. **연결 종료 처리 부재:** 클라이언트의 연결이 끊어졌을 때, 해당 플레이어를 `Room`에서 제거하는 로직이 없어 유효하지 않은 세션 포인터가 계속 남아있게 되는 문제가 있었습니다.

초기에 논블로킹(Non-blocking) I/O 모델 도입을 검토했으나, **"최대 플레이어 3명"** 이라는 명확한 제약 조건 하에서는 '클라이언트별 스레드' 모델의 복잡성 대비 실익이 적다고 판단하여, 현재 모델을 유지하되 안정성과 정확성을 높이는 방향으로 최종 결정했습니다.

## 2\. 아키텍처 개선 결정 사항

위 문제들을 해결하기 위해 다음과 같은 아키텍처 개선을 진행했습니다.

1. **'클라이언트별 스레드' 모델 유지 및 안정화:** 최대 클라이언트 수가 3명으로 고정되어 있으므로, 복잡한 I/O 모델 대신 가장 직관적인 현재 모델을 유지하되, 블로킹 `recv` 로직을 안정적으로 다듬어 사용하기로 결정했습니다.
2. **게임 루프 전용 스레드 도입:** 게임 로직의 실행을 보장하고 I/O 처리와 명확히 분리하기 위해, `Room`의 `UpdateGame` 함수를 실행하는 전용 스레드(`\_roomThread`)를 `Server` 클래스에 추가했습니다.
3. **세분화된 락(Fine-Grained Locking) 적용:** 단일 `CRITICAL\_SECTION`으로 모든 공유 자원을 관리하던 방식에서 벗어나, `Players` 벡터와 `incomingQueue`를 각각 별도의 `std::mutex` (`\_players\_mutex`, `\_queue\_mutex`)로 보호하도록 변경했습니다. 이를 통해 락 경합을 줄여 병목 현상을 개선하고, `std::lock\_guard`를 사용해 RAII 패턴을 적용함으로써 코드의 안정성을 대폭 향상시켰습니다.

## 3\. 상세 구현 변경 내역

### 3.1. `Server` 클래스 (`Server.h`, `Server.cpp`)

* `Room` 객체(`\_room`)와 게임 루프 스레드(`\_roomThread`)를 멤버 변수로 추가했습니다.
* `Server::start()`: `listen()` 호출 후, `\_roomThread`를 생성하여 `\_room.UpdateGame()`을 실행하도록 변경했습니다. 이로써 서버 시작과 함께 게임 루프가 동작하게 됩니다.
* `Server::end()`: 모든 스레드가 종료되기 전, `\_roomThread.join()`을 호출하여 게임 루프 스레드가 안전하게 종료될 때까지 대기하도록 수정했습니다.
* `Server::acceptLoop()`: 새로운 클라이언트 접속 시, `\_room.AddPlayer()`와 `session.setCurrentRoom()`을 호출하여 `Session`과 `Room` 객체를 명확히 연결했습니다.

### 3.2. `Session` 클래스 (`Session.h`, `Session.cpp`)

* `setCurrentRoom(Room\* room)`: `Server`가 `Session`에 `Room`의 주소를 알려줄 수 있는 public 메서드를 추가했습니다.
* `RecvPacket()`: `recv()` 함수의 반환값을 명확히 체크하여 소켓 오류나 정상 종료(`retval == 0`) 시, `Disconnect()`를 즉시 호출하도록 수정했습니다. 이를 통해 세션 스레드가 비정상 상태에서 무한 루프에 빠지는 것을 방지하고 깔끔하게 종료되도록 보장합니다.
* `Disconnect()`: `closesocket()` 호출 전에 `\_currentRoom->RemovePlayer(this)`를 호출하여, `Room`에 자신의 제거를 요청하는 로직을 추가했습니다.

### 3.3. `Room` 클래스 (`Room.h`, `Room.cpp`)

* **게임 루프 `UpdateGame()` 전면 수정:**

  * `\_isGameRunning` `atomic<bool>` 플래그를 추가하여 게임 루프의 시작과 종료를 제어할 수 있도록 했습니다. (`StartGame`, `StopGame` 함수 추가)
  * **고정 시간 스텝(Fixed Timestep)** 로직을 도입했습니다. `Timer`를 사용하여 약 30 FPS 주기로 루프가 돌도록 `sleep`을 추가하여, 일관적인 게임 시뮬레이션과 CPU 자원 효율성을 확보했습니다.
  * 루프의 단계를 **`ProcessInputs` -> `UpdateState` -> `BroadcastState`** 로 명확히 분리했습니다.

* **스레드 안전성 확보:**

  * `CRITICAL\_SECTION`을 `std::mutex \_players\_mutex`와 `std::mutex \_queue\_mutex`로 교체했습니다.
  * `AddPlayer`, `RemovePlayer`, `BroadcastState`, `BroadcastPacket` 함수는 `\_players\_mutex`로, `EnqueuePacket`, `ProcessInputs` 함수는 `\_queue\_mutex`로 보호하여 각 공유 자원에 대한 접근을 동기화했습니다.
  * 모든 락킹(locking)은 `std::lock\_guard`를 사용하여 예외 발생 시에도 락이 안전하게 해제되도록 구현했습니다.

* **상태 관리 및 전송 로직 수정:**

  * `ProcessInputs()`: 이제 입력 큐의 모든 패킷을 처리하여 서버 내부의 `Player` 상태를 **업데이트하는 역할만** 수행합니다.
  * `BroadcastState()`: `ProcessInputs`와 `UpdateState`가 끝난 후, `\_players\_mutex`로 `Players` 벡터를 안전하게 순회하며 **모든 플레이어의 최신 상태를 취합**하여 `S2C\_AllPlayerMovePacket` 하나를 만듭니다.
  * `BroadcastPacket()`: `BroadcastState`로부터 받은 패킷을 모든 플레이어에게 전송할 때, 각 플레이어마다 `new char\[packet->size]`와 `memcpy`를 통해 **패킷의 복사본을 생성**하여 전달합니다. 이를 통해 각 `Session` 스레드가 다른 스레드와 무관하게 패킷 메모리를 `delete` 할 수 있도록 하여 메모리 관련 버그를 원천 차단했습니다.

## 4\. 최종 아키텍처 요약

리팩토링을 통해 서버는 다음과 같은 안정적인 구조를 갖추게 되었습니다.

* **모델:** '클라이언트별 스레드' 모델 (최대 3명)
* **I/O:** 안정적인 블로킹(Blocking) `recv`
* **스레드 구성:**

  1. **메인 스레드 (1개):** 클라이언트 접속 처리 (`accept`)
  2. **세션 스레드 (클라이언트당 1개):** 각 클라이언트의 데이터 수신(`recv`) 및 `Room`의 큐에 패킷 전달
  3. **게임 루프 스레드 (1개):** `Room`의 `UpdateGame`을 실행하며, 고정된 주기로 모든 입력 처리, 게임 상태 업데이트, 결과 브로드캐스팅을 수행

* **동기화:** `std::mutex`와 `std::lock\_guard`를 사용하여 공유 자원(`Players` 벡터, `incomingQueue`)에 대한 접근을 안전하게 관리

이 구조는 소규모 다중 접속 환경에서 직관적이고 안정적으로 동작하며, 기능 추가 및 유지보수가 용이한 기반을 마련합니다.

