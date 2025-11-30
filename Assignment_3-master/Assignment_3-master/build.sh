if [ ! -d "bin" ]; then
    mkdir bin
else
	rm bin/*
fi

g++ -g -O0 -I . -o bin/interrupts_EP interrupts_EhimareIsoa_AshfaqulAlam_EP.cpp
g++ -g -O0 -I . -o bin/interrupts_RR interrupts_EhimareIsoa_AshfaqulAlam_RR.cpp
g++ -g -O0 -I . -o bin/interrupts_EP_RR interrupts_EhimareIsoa_AshfaqulAlam_EP_RR.cpp