### 목표
* Ext4 파일 시스템의 구조 알아오기
* Ext 파일 시스템에서 데이터를 숨길 수 있는 영역 찾아보기

### Ext 파일 시스템의 구조
Ext4 파일 시스템에서 데이터 저장의 최소 단위는 블록으로, 보통 4KB를 사용한다.
또한, 각 파일이나 디렉터리는 고유한 Inode를 가지며, Inode는 파일 또는 디렉터리의 크기, 소유자, 권한, 생성 및 수정 시간 등의 메타데이터가 저장된다. 
즉, Inode는 존재하는 파일에 대한 정보를 담고 있는 데이터라 할 수 있다.

![[Pasted image 20250710022406.png]]
Ext4에서는 이러한 데이터들을 효과적으로 관리하기 위해 모든 블록들을 블록 그룹으로 묶어서 관리하며, 여러 개의 블록 그룹이 모여 전체 파일 시스템을 구성한다. 
* 하나의 블록 그룹 = 최대 32,768개의 블록
* 블록 하나의 크기가 4KB일 경우, 블록 그룹 하나의 크기 = 최대 128MB
* Ext4 파일시스템 파티션의 크기가 1GB일 경우, 8개의 블록 그룹이 존재

##### Super block
0번째, 홀수 번째 블록 그룹에 존재하며 파일 시스템의 전체적인 정보를 저장하는 부분으로, 블록의 크기, 전체 블록의 개수, Inode table의 크기, 각 블록 그룹 당 inode table의 개수 등의 정보를 저장한다.
전체 크기는 1024 바이트이다. 

##### Group Descriptors
Bitmap 공간의 위치, 사용 가능한 블록의 수, 블록 내의 디렉터리 수 등의 정보를 기록하며, 64 바이트를 차지한다. 
![[Pasted image 20250710023141.png]]
![[Pasted image 20250710023154.png]]
##### Block / Inode bitmap
각각 해당 블록 그룹 내부의 블록/Inode들의 할당 정보를 비트 단위로 매핑하여 저장한다. 
##### Inode table
* 해당 블록 그룹 안의 모든 데이터에 대한 inode table들이 번호에 따라 순서대로 존재한다. 
* 각각의 Inode table에는 시간 정보와 데이터의 크기, 데이터가 존재하는 블록의 포인터 등이 존재한다. 
##### Journal log area
파일 시스템에서 발생한 **트랜잭션**들에 대한 로그들이 존재하며, 이 영역의 위치와 크기는 Super block에 저장된다. 
또한 Journal 안의 모든 블록은 12 바이트 헤더로 시작하며, 그 값은 다음과 같다. ![[Pasted image 20250710023859.png]]
##### Data block
데이터의 Inode 번호, 이름 등의 메타데이터가 저장되어 있는 블록과 실제 데이터가 들어있는 블록들이 존재하며, 이 영역에 실제 데이터가 들어 있다. 
##### MMP
Multiple Mount Protection의 약자로, 파일 시스템이 여러 호스트에 의해 동시에 사용되는 것을 방지한다. 
파일 시스템을 생성할 때 mke2fs 뒤에 `-0 mmp` 문구를 추가하여 기능을 활성화할 수 있다. ![[Pasted image 20250710024132.png]]

### Ext 파일 시스템의 특성
##### 파일 할당
Ext4 파일 시스템에서 실제 파일의 할당 정보는 Inode table 내부의 extents 구조체를 사용하여 저장된다. 
하나의 구조체는 12 바이트로, 이 구조체를 통해 실제 데이터가 몇 번 offset으로부터 몇 개의 블록을 할당하여 저장되었는지 알 수 있다. 
최초의 extents 앞에는 12 바이트 크기의 extents header가 존재하여 총 몇 개의 extents가 있는지 알 수 있다. 
>[!example]- extents header와 extents의 구조 (클릭)![[Pasted image 20250710025733.png]]
![[Pasted image 20250710025746.png]]

이 때 데이터의 크기가 블록의 배수가 아닐 경우, 마지막 블록의 미할당 영역을 0x00으로 초기화하여 데이터가 어디서 끝나는지 판단할 수 있게 한다. 

또한 저장하려는 파일이 파일 시스템에 의해 대용량 파일로 분류되거나 단편화된 파일인 경우, 4개 이상의 extents 구조체가 필요할 수 있다.
이 경우는 extents tree를 사용하여 데이터를 저장하며, tree의 깊이에 따라 extents에 존재하는 정보가 달라진다. 
* 깊이가 0인 경우: extents 구조체에 데이터가 존재하는 실제 블록의 offset이 저장
* 깊이가 1 이상인 경우: index node에 실제 데이터 블록의 offset을 참조하는 값이 저장
	* 깊이가 1일 경우: index node에 실제 데이터 블록의 offset이 저장
	* 깊이가 2일 경우: index node에 leaf node의 offset이 저장, leaf node에 실제 데이터 블록의 offset이 저장
	![[Pasted image 20250710030408.png]]
##### 단편화
Ext4 파일 시스템은 파일의 단편화(공간이 없어 파일이 조각조각 나누어 저장되는 현상)를 최소화하기 위해 파일을 최대한 연속적인 블록에 저장한다. 
이 때문에 하나의 파일은 최대 127MB까지 단편화가 발생하지 않으나, 그럼에도 불구하고 단편화가 되는 경우가 2가지 존재한다. 

1. 생성되는 파일의 크기가 127MB를 초과하는 경우
2. 이미 존재하는 파일이 계속 수정되어 기존의 저장 공간에 연속적으로 저장되어 단편화되는 경우

이 경우 조각들을 해당 파일과 가장 가까운 곳에 위치한 같은 블록 그룹의 비할당 영역에 저장한다. 

##### Flexible Block Groups
Ext4에서 신규 사용된 기능이며, 여러 개의 블록 그룹이 하나의 논리적인 블록 그룹으로 사용된다. 
이 경우, Block bitmap, Inode bitmap, Inode table 공간이 몇 개씩 묶여 하나의 블록 그룹 안에 들어간다. 
* '몇 개씩'의 기준: Super block에 있는 `s_log_groups_per_plex`의 값에 달려있음
* 한 블록 내에 같이 묶이는 Bitmap 공간 & Inode table의 개수 (2의 거듭제곱)$$2^(s-log-groups-per-flex)$$![[Pasted image 20250710025017.png]]
##### Boot Block
Boot Code를 처음 1024 바이트에 두기 위하여, 첫 번째 블록 그룹에 대해서는 처음 1024 바이트는 사용하지 않는다. 

### 데이터를 숨길 수 있는 영역
* 메타데이터: Inode에서 파일의 속성, 권한, 타임스탬프 등의 영역에 은닉
* 저널링 영역: Journal Log Area 영역은 파일 시스템의 변경 내역을 기록하는 데 사용하고 일반적인 파일 탐색기로는 접근할 수 없어 데이터를 숨길 수 있음
* 미사용 영역: 파일 시스템의 빈 공간을 활용하는 것으로, Inode가 접근하지 않으면 데이터가 직접 파일로 연결되지 않아 탐지하기 어려움

### 4KB의 파일을 생성 및 삭제하는 과정
##### 생성한 경우
1. Block bitmap을 기반으로, 사용 가능한 블록을 탐색한다
2. inode bitmap 정보를 기반으로 사용 가능한 inode를 찾고, 해당 inode에 생성된 파일의 정보를 저장
3. 생성한 파일에 할당된 데이터 블록과 inode가 포함된 inode block을 동시에 스토리지에 반영한다. 

##### 삭제한 경우
데이터가 들어있는 블록에 대하여만 Trim 명령어를 스토리지로 전송하며, 삭제된 파일의 inode가 포함된 inode table의 블록은 그대로 유지한다. 
이는 동일한 inode 블록에 포함된 inode들 중 일부는 유효한 파일의 메타데이터를 포함할 수도 있기 때문으로, inode 블록 전체를 삭제하여 유효한 파일의 메타데이터를 손실하지 않기 위함이다. 
(현재의 Ext4 파일 시스템은 16개의 inode가 전부 무효하다 해도 해당 inode 블록에 대한 Trim 명령어를 스토리지에 전송하지 않고, 무효화된 inode table의 정보를 그대로 유지한다.)

![[Pasted image 20250710031202.png]]
### 기타 정리
- Orphan file
    - Orphan inode entries: 각 블록은 orphan inode 엔트리 배열을 포함하며, 각 엔트리는 Inode 번호를 저장, 비어 있는 경우 0으로 표시
    - ob_magic: 블록이 유효한지 확인하는 데 사용
    - ob_checksum: 블록의 무결성을 확인하기 위해 checksum이 포함됨

- h_blocktype of Block Header
    - 1: Transaction 앞에 옴.
    - 2: Transaction 뒤에 와서 Transaction의 끝을 알림
    - 5: 블록 취소 기록으로, 이렇게 하면 나중에 다시 쓰여진 블록을 저널에서 건너뛸 수 있어 복구 속도가 빨라짐. 즉 A, B, C를 쓰고 B를 지우고 난 후 시스템 에러가 생겼을 때, 복구 과정에서 B가 다시 작성되지 않도록 하여 파일 시스템의 일관성과 속도 면에서 이점이 있음.

    - **트랜잭션**: 일련의 작업을 하나의 단위로 묶어서 처리하며, 모든 작업이 성공적으로 수행되거나 모두 실패하는 것을 보장함.
        → 시스템 오류나 예기치 않은 중단 상황에서 데이터의 일관성을 유지할 수 있음
        
    - 트랜잭션의 주요 역할
        1. 원자성(Atomicity): 트랜잭션 내 모든 작업이 성공적으로 완료되거나, 하나라도 실패하면 전체 트랜잭션이 취소 → 부분적인 데이터 업데이트로 인한 불일치를 방지
        2. 일관성(Consistency): 트랜잭션이 시작되기 전과 완료된 후의 데이터 상태가 일관성을 유지하도록 보장, 트랜잭션이 성공적으로 완료되면 데이터는 항상 유효한 상태로 유지
        3. 격리성(Isolation): 동시에 실행되는 여러 트랜잭션이 서로 간섭하지 않도록 격리 → 트랜잭션 간의 충돌을 방지, 데이터의 무결성을 유지
        4. 지속성(Durability): 트랜잭션이 성공적으로 완료되면 그 결과는 영구적으로 저장, 시스템 오류가 발생하더라도 트랜잭션의 결과는 손실되지 않음
        5. 
- MMP의 동작 원리
    - 파일 시스템을 열 때(마운트되거나 fsck(파일 시스템 검사)가 실행될 때), 해당 노드에서 실행 중인 MMP 코드가 시퀀스 번호를 확인, 다른 노드가 같은 파일 시스템을 사용하고 있는지 확인
        - 시퀀스 번호가 **EXT4_MMP_SEQ_CLEAN**일 경우: 파일 시스템을 열 수 있음
        - 시퀀스 번호가 **EXT4_MMP_SEQ_FSCK**일 경우: 파일 시스템 검사가 실행되고 있다는 의미 ⇒ 파일 시스템을 열 수 없음

### 참고 사이트 및 참고 문헌
##### 참고 사이트
[Ext4 Disk Layout- Ext4](https://archive.kernel.org/oldwiki/ext4.wiki.kernel.org/index.php/Ext4_Disk_Layout.html)
[ext4 Data Structures and Algorithms - The Linux Kernel Documentation](https://www.kernel.org/doc/html/latest/filesystems/ext4/)
[2. Data Structures and Algorithms - The Linux Kernel Documentation](https://www.kernel.org/doc/html/v4.19/filesystems/ext4/ondisk/index.html)
[ext4(5) - Linux manual page](https://www.man7.org/linux/man-pages/man5/ext3.5.html)
##### 참고 문헌
>[!example]- Journaling of Journal 기반 SQLite 파일 복구 기법
>![[Journaling of Journal 기반 SQLite 파일 복구 기법 1.pdf]]

>[!example]- 안드로이드 운영체제의 Ext4 파일 시스템에서 삭제 파일 카빙 기법
>![[안드로이드 운영체제의 Ext4 파일 시스템에서 삭제 파일 카빙 기법.pdf]]

>[!example]- 플래시 스토리지 기반 Ext4 파일 시스템의 효율적인 파일 삭제 기법
>![[플래시 스토리지 기반 Ext4 파일시스템의 효율적인 파일 삭제 기법.pdf]]
