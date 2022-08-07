# TODO : 
## Configuration:
- limit the client body (use curl -X POST -H "Content-Type: plain/text" --data "BODY IS HERE write something shorter or longer than body limit")

- setup multiple servers with different hostname (use something like: curl --resolve example.com:80:127.0.0.1 http://example.com/)

- UNKNOWN requests -> should not produce any crash

## CGI:
- cgi headers
- cgi file path 

## Upload: 
- upload some file to the server and get it back
(upload path should be at the root folder to be able to get back files by requests)

## NO MEMEORY LEAK ()
- Check if there is no memory leak (monitor the process memory usage it should not go up indefinitely)()