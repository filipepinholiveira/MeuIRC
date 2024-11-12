P
ara implementar um servidor IRC em C++ 98, conforme as instruções que você forneceu, o objetivo é criar um servidor que escute por conexões de clientes em uma porta especificada e aceite apenas conexões que forneçam uma senha correta. O servidor deve ser capaz de gerenciar múltiplos clientes simultaneamente, e para isso, podemos usar a função select() para monitorar múltiplos sockets de maneira eficiente.

Estrutura Básica do Servidor IRC
O servidor escutará uma porta fornecida como argumento de linha de comando.
O servidor deverá aceitar conexões de clientes e verificar a senha antes de permitir a comunicação.
O servidor precisa lidar com múltiplos clientes de maneira concorrente, sem usar threads, mas utilizando select() para monitorar os sockets de maneira assíncrona.
O servidor deve ser capaz de ler e enviar mensagens para os clientes conectados.

Fluxo Geral:

Criar o socket do servidor.
Associar o socket à porta fornecida.
Esperar por conexões de clientes usando select() para gerenciar múltiplas conexões simultaneamente.
Verificar a senha ao aceitar a conexão de um cliente.
Ler mensagens enviadas pelos clientes e retransmitir para todos os clientes conectados.
Fechar a conexão quando o cliente desconectar.

