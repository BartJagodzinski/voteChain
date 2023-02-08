votechain: checker node voter

checker: checker.cpp
	g++ checker.cpp -o checker -lboost_system -pthread -std=c++17

node: node.cpp
	g++ node.cpp -o node -lboost_system -pthread -std=c++17

voter: vote_sender.cpp
	g++ vote_sender.cpp -o voter -lboost_system -lsecp256k1 -pthread -std=c++17

macOS: macOSchecker macOSnode macOSvoter

macOSchecker: checker.cpp
	g++ checker.cpp -o checker -lboost_system-mt -pthread -std=c++17

macOSnode: node.cpp
	g++ node.cpp -o node -lboost_system-mt -pthread -std=c++17

macOSvoter: vote_sender.cpp
	g++ vote_sender.cpp -o voter -lboost_system-mt -lsecp256k1 -pthread -std=c++17
