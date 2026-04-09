## 게임 루프 및 UI

+ GameMode, GameController, Pawn, IMC, IA 를 이용한 3D캐릭터
+ 아이템 인터페이스, BaseItem을 상속받아서 Coin, HealingPotion, Mine 아이템 구현
+ SpawnVolume 바운딩박스 내 랜덤한 위치에 누적합을 이용한 랜덤 아이템 생성
+ 아이템 데이터 테이블을 이용해서 레벨 별 다른 스폰 확률 적용
+ Overlap 이벤트를 이용한 충돌처리 및 다형성을 이용한 아이템 개별 처리
+ GameState를 이용한 레벨 처리 및 GameInstance 이용한 전역 데이터 관리 (GameState 내 변수 제거 필요함)
+ 
<br><br>

