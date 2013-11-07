------------- 일반적인 git 사용 방법 ------------

0. 먼저 git pull car master 해서 코드를 최신버전으로 맞춰놓는다. ( 코드 확인 하고 난 뒤에 )
1. 파일 수정
2. git 폴더에서(dijkstra) git add . -A 혹은 git add "파일명"
3. git commit -m "수정한 내용 or Comment"
4. git push "remote이름" "branch이름" -> ex) git push car master
5. 끄읕
6. 만약 코드가 최신버전이 아니라면 push가 되지 않으므로 주의하삼

-------------- branch 사용 방법 --------------

1. git checkout -b "branch 이름" -> ex) git checkout -b "binensky"
2. 파일 수정
3. 파일 실행하고 잘 되면 master로 합친다.
	1) git checkout master ( master로 checkout 함 )
	2) git merge "branch 이름" "master" -> ex) git merge binensky master
4. 끄읕

5. 만약 Auto Merge 안되고 CONFLICT 나면 CONFLICT 된 파일에 가서 수동으로 고친다.
6. 끄읕


-------------- 항상 push 할땐 add . -A 후 commit
