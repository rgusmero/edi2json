LIB_EDI=./libedi
LIB_JSON=./json

edi2json: libedi.so libjson.so edi2json.c
	gcc edi2json.c libedi.so -I$(LIB_EDI) libjson.so -I$(LIB_JSON) -o edi2json
	
libedi.so: 
	gcc -shared -o libedi.so $(LIB_EDI)/*.c

libjson.so:
	gcc -shared -o libjson.so $(LIB_JSON)/*.c
	
clean:
	rm -f *.o *.so edi2json edi2json.exe
	
