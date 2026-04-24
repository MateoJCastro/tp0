#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	log_info(logger, "Hola! Soy un Log");


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	if (config == NULL){

		log_error(logger, "No se pudo crear el archivo config");
		abort();
	
	}

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config,"CLAVE");


	log_info(logger,"El valor de la clave es %s", valor);


	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor

	conexion = crear_conexion(ip, puerto);

	// --- INICIO DEL HANDSHAKE ---
	size_t bytes;
	int32_t handshake = 1;
	int32_t result;

	// Mandamos nuestro "Hola" (el número 1)
	bytes = send(conexion, &handshake, sizeof(int32_t), 0);

	// Esperamos la respuesta del servidor
	bytes = recv(conexion, &result, sizeof(int32_t), MSG_WAITALL);

	if (result == 0) {
    	log_info(logger, "Handshake exitoso. El servidor me aceptó.");
	} else {
    	log_error(logger, "Error en el handshake. El servidor me rechazó.");
    	abort(); // Si falla el saludo inicial, cortamos todo
	}
	// --- FIN DEL HANDSHAKE ---

	// Enviamos al servidor el valor de CLAVE como mensaje

	enviar_mensaje(valor,conexion);

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;

	nuevo_logger = log_create("tp0.log","CLIENTE",1,LOG_LEVEL_INFO);

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;

	nuevo_config = config_create("cliente.config");

	return nuevo_config;
}


void paquete(int conexion)
{
	char* leido;
	t_paquete* paquete;

	paquete = crear_paquete();
	
	leido = readline("> ");

	while (strcmp(leido, "") != 0) {

		agregar_a_paquete(paquete, leido, strlen(leido) +1);
		free(leido);
		leido = readline("> ");

	}

	free(leido);

	enviar_paquete(paquete,conexion);

	eliminar_paquete(paquete);

	
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{

	close(conexion);
	log_destroy(logger);
	config_destroy(config);


}
