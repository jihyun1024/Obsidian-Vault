앱 사이에서 각종 데이터를 공유할 수 있도록 하는 컴포넌트이다. 
안드로이드 표준 시스템에서는 연락처인 Contacts나 이미지, 동영상 등의 데이터를 보관하는 MediaStore 등이 있다. 
데이터를 검색, 추가, 갱신, 삭제할 수 있으며, 주로 SQLite 등의 관계형 데이터베이스 이용을 염두에 두고 제작되었다. 
### ContentProvider로부터 데이터를 읽어오기
ContentProvider로부터 데이터를 읽어오려면 해당 ContentProvider가 어디에 있는지 알아야 한다. 
경로는 "content://스키마"를 가진 [[URI(Universal Resource Identifier)|URI]]로 지정되고, 일반적으로 접근할 앱에서 정의한다. 
이 URI는 Authority로 불리고, ContentProvider를 직접 만들 때는 AndroidManifest.xml에 기술하게 된다.

### ContentResolver를 통해 데이터를 읽기
ContentProvider가 제공하는 데이터에는 ContentResolver를 통해 접근하도록 설계되어 있다. 
ContentResolver의 인스턴스는 getContentResolver() 메서드로 가져오게 된다. ![[Pasted image 20250710155334.png]]
ContentResolver에 [[URI(Universal Resource Identifier)|URI]]를 전달함으로써 ContentProvider의 데이터에 접근할 수 있다. 