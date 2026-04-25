#include "server.h"

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

		// --- INICIO DEL HANDSHAKE ---
	size_t bytes;
	int32_t handshake;
	int32_t resultOk = 0;
	int32_t resultError = -1;

		// Nos quedamos esperando a que el cliente nos mande su código de presentación
	bytes = recv(cliente_fd, &handshake, sizeof(int32_t), MSG_WAITALL);
	if (bytes <= 0) {
    	log_error(logger, "Error al recibir el handshake, o el cliente se desconectó");
    	return EXIT_FAILURE; // Cortamos la ejecución de este cliente
	}

	if (handshake == 1) {
    	// Si nos mandó un 1, le decimos que todo OK (0)
    	bytes = send(cliente_fd, &resultOk, sizeof(int32_t), 0);
    	log_info(logger, "Handshake con cliente exitoso!"); // Opcional si tenés logger acá
	} else {
    	// Si nos mandó cualquier otra cosa, le cortamos el rostro (-1)
    	bytes = send(cliente_fd, &resultError, sizeof(int32_t), 0);
    	log_warning(logger, "Handshake fallido!");
	}
	// --- FIN DEL HANDSHAKE ---

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
