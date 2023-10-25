# 에러 리스트
### 모드 처음 들어갔을 때 에러메시지 뜨는 거 확인 
### 같은 방 안에서 서로 대화 안됌
- 처음에는 다른 방이 만들어지나 했지만, 기존 
### ~~join 메시지- topic없을경우 처리(시원,, 간단한거라 내일 가서 바로 해둘게)~~
### part시 s->c로 가는 메시지
irssi로 처리 안돼고 있어서, 채널에 있는 유저리스트에서는 지워졌으나 irssi에서는 방 나간게 안 보임

### part : 두번 해야 나감... 일단 part 두번 해서 처리해둠??
- 한번만에 정상처리시 나온 메시지
:tt!root@root PART #a
:tt QUIT :Quit: : from this channel
- 비정상처리시 메시지
:tt!root@root PART #a
:tt QUIT :Quit: : from this channel