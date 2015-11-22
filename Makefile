compile: main.cpp src/card.cpp src/error.cpp src/game.cpp src/input.cpp src/iplayer.cpp src/smartRandom.cpp src/random.cpp src/trick.cpp KoenHidde/KoenHidde_src/CardData.cpp KoenHidde/KoenHidde.cpp KoenHidde/KoenHidde_src/Decide.cpp KoenHidde/KoenHidde_src/PassCards.cpp 
	mkdir -p bin
	g++ main.cpp src/card.cpp src/error.cpp src/game.cpp src/input.cpp src/iplayer.cpp src/smartRandom.cpp src/random.cpp src/trick.cpp -I. -Iheaders -IKoenHidde/KoenHidde_headers KoenHidde/KoenHidde_src/CardData.cpp KoenHidde/KoenHidde.cpp KoenHidde/KoenHidde_src/Decide.cpp KoenHidde/KoenHidde_src/PassCards.cpp -O2 -o bin/Hearts
 
