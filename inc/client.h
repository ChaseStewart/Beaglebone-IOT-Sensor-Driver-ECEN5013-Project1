

#define CHAR_RET   10
#define CHAR_NEWLN 13

int client_state;

void handleSigint(int sig);

int main(int argc, char **argv);

int clientLogMessage(message_t *in_message);


/* send temperature request*/
int clientsendTemp(mqd_t temp_queue);

/* send light request */
int clientSendLight(mqd_t light_queue);

/* send log request */
int clientSendLog(mqd_t logger_queue);

/* send the shutdown*/
int clientSendShutdown(mqd_t main_queue);

