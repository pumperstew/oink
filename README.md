# oink
A rudimentary chess engine built for fun.

I've wanted to write a chess engine for a while, and fancied implementing it in C++ using bitboards as the representation, since these looked fun to try to wrap my head around.
This is just a fun project, and pretty much everything has been done from scratch.

Oink currently plays a legal game and can sometimes beat me at blitz. Almost no work has gone into the evaluation function -- it's
purely materialistic.

Oink is written in C++ and uses cmake for build configuration and googletest for testing. It's currently only built on Windows.
