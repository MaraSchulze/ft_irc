# ft_irc
This was a team project for the 42Heilbronn coding school.  
We programmed a simple IRC server in C++ that can nevertheless be connected to one or several KVIrc clients.  
The subject with the detailed description is in the pdf file.  

## Supported commands
PASS, NICK, USER, JOIN, PART, PRIVMSG, QUIT, KICK, INVITE, TOPIC, MODE itkol  

## Usage
This project was written and compiled for a Mac.  
```
make
./ircserv <port> <password>
```

## Caveats
We had to program adhering to the C++ standard 98.  

## Thanks to
the lovely Yalda Golshani (ygolshan)!  
I programmed the application layer and Yalda programmed the socket layer.  
