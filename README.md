# Webservo 
# TO_DO_LIST:
- Select : https://linux.die.net/man/2/poll / vs / https://en.wikipedia.org/wiki/Select_(Unix)
- handle each file type depend on authorized ones , and add the reqiured infos on response header 

- [fixed] garbadge value on buffer read from client SOCKET 
- [fixed] abort after server been broken 


- [ ] content-type not parsed correctly there is (\r \n space).
- [ ] structure files   
- [ ] bad splite with : with headeadrs in rewuest (referer) 

- [to_fix] fix the downoald/recv of body file with using of content-lenght + index-of-body - be careful of is_shunked 
- [to_fix]  fix the upload/send of response header and body by buffer size
