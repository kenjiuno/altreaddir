alt.so: alt.cpp
	gcc -shared -fPIC alt.cpp -ldl -lstdc++ -o alt.so
