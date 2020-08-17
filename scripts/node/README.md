# Scripts auxiliares (Node.js)

Antes de executar qualquer script, instale primeiro as dependencias do projeto com o comando `npm install`

## generate-docs-asyncapi.sh

Esse script lê o arquivo `data/asyncapi/asyncapi.yaml` e gera a documentação em HTML do AsyncAPI 2.0.

Se for executado com sucesso, ele remove a documentacao antiga da pasta `data/asyncapi/` e insere a nova.

**Execute esse script com `npm run generate-docs-asyncapi`**

---

## udp-server.js

Esse script inicia um servidor UDP na porta 2222.

Inicie esse servidor sempre que precisar ler os logs remotos do ESP8266.

**Execute esse script com `node udp-server.js`**

---

## udp-client.js

Esse script inicia um cliente UDP. É util para testar se o servidor UDP está funcionando corretamente.

**Execute esse script com `node udp-server.js`**

