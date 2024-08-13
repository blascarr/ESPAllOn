#include <Arduino.h>
#include <unity.h>

#include "../config.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

void executeDelayedCommand() {
	// Espera 10 segundos
	std::this_thread::sleep_for(std::chrono::seconds(10));
	// Ejecuta el comando después de la espera
	int returnCode = system("node ../node_test/src_test/puppeteer_test.js");
	std::cout << "Código de retorno: " << returnCode << std::endl;
}

int main() {
	std::cout << "El hilo principal continúa ejecutándose..." << std::endl;
	// Lanza el hilo que maneja la espera y la ejecución del comando
	std::thread worker(executeDelayedCommand);
	// Hace que el hilo principal espere a que el trabajador termine
	worker.join();
	std::cout << "El hilo de ejecución ha completado." << std::endl;
	return 0;
}

void setup() {
	UNITY_BEGIN();
	if (PRINTDEBUG)
		SERIALDEBUG.begin(115200);
	// RUN_TEST(test_esp_mac);
	UNITY_END();
}

void loop() {}