<img src="./docs/images/globe.png" alt="Icone Globo" title="Aimeos" align="right" height="90" />
<img src="./docs/images/iot.png" alt="Icone IoT" title="Aimeos" align="right" height="90" />

Globo Terrestre IoT
======================

![License MIT](https://img.shields.io/badge/license-MIT-blue.svg)

Um projeto usando o ESP8266 ESP-12E Nodemcu.

Este projeto foi construido sobre minha necessidade de entender melhor a comunicação por Wi-Fi dos microcontroladores ESP8266.

E realizar a integração desse microcontrolador com Windows, OSX, Linux, iOS e Android.

---

### Tecnologias exploradas no projeto:

    - Wi-Fi
    - IP e Porta
    - Atualização Over-The-Air (OTA)
    - Transmission Control Protocol (TCP)
    - User Datagram Protocol (UDP)
    - Hypertext Transfer Protocol (HTTP)
    - WebSocket
    - multicast DNS (mDNS)

<table align="center">
  <tr >
    <th>Tecnologia</th>
    <th>Ultilidade no projeto</th>
  </tr>
  <tr align="center">
    <td>Wi-Fi</td>
    <td>Troca de informações sem fio entre dispositivos.</td>
  </tr>
  <tr align="center">
    <td>IP e Porta</td>
    <td>Endereçar o dispositivo na rede e chamar serviço sendo executado em uma porta.</td>
  </tr>
  <tr align="center">
    <td>OTA</td>
    <td>Atualização/Reprogramação do ESP8266 de forma remota usando apenas o Wi-Fi.</td>
  </tr>
  <tr align="center">
    <td>UDP</td>
    <td>Protocolo usado para debugar remotamente o projeto.</td>
  </tr>
  <tr align="center">
    <td>HTTP</td>
    <td>Protocolo usado nas requisões da API implementada e páginas hospedadas no ESP8266.</td>
  </tr>
  <tr align="center">
    <td>WebSocket</td>
    <td>Usado para realizar uma conexão quase em tempo real de baixa latência entre o cliente e o ESP8266.</td>
  </tr>
  <tr align="center">
    <td>mDNS</td>
    <td>Para não digitar sempre o IP do ESP8266, ultiliza-se o mDNS. Ex: http://globo.local</td>
  </tr>
</table>

### Resumo sobre as tecnologias usadas

#### Pilha de protocolos TCP/IP

<table align="center">
  <tr >
    <th>Camada</th>
    <th>Protocolos</th>
  </tr>
  <tr align="center">
    <td>Aplicação</td>
    <td>HTTP, WebSocket, mDNS ...</td>
  </tr>
  <tr align="center">
    <td>Transporte</td>
    <td>TCP, UDP</td>
  </tr>
  <tr align="center">
    <td>Rede</td>
    <td>IP</td>
  </tr>
  <tr align="center">
    <td>Interface de Rede</td>
    <td>Ethernet, Wi-Fi ...</td>
  </tr>
</table>

#### Atualização Over-The-Air (OTA)

O Over-The-Air não é um protocolo e sim uma forma de enviar um Firmware para o dispositivo sem necessidade de cabos. O OTA permite reprogramar o microcontrolador através da rede Wi-Fi sem a necessidade de removê-lo do local que ele está instalado.

#### Transmission Control Protocol (TCP)

O protocolo TCP é mais utilizado por oferecer confiabilidade durante a comunicação. O TCP garante que os pacotes de dados trafegados permaneçam inalterados e que cheguem na mesma ordem com que foram enviados.

#### User Datagram Protocol (UDP)

O UDP é um protocolo que não garante a chegada dos dados. Por esse motivo o UDP é mais rápido que o TCP, pois não realiza a verificação de erros nos pacotes de dados.

#### Hypertext Transfer Protocol (HTTP)

O HTTP é um protocolo unidirecional (a transmissão ocorre só de uma ponta para a outra), onde o cliente envia a requisição e o servidor retorna a resposta, finalizando ali a conexão.

#### WebSocket

WebSocket é um protocolo que permite a criação de um canal de comunicação cliente-servidor com transmissão bidirecional onde ambos os lados (cliente e servidor) podem transmitir dados simultaneamente. WebSocket veio para suprir as deficiências do protocolo HTTP para esse objetivo.

#### multicast DNS (mDNS)

O mDNS é um protocolo DNS Multicast. Um "multicast" repassa a mesma mensagem para vários pontos em uma rede. O mDNS é um método para descoberta de vizinhos de uma rede, indicado para redes pequenas sem servidores DNS próprios.

---

### Como usar

*⚠️ A estrutura do projeto não é reconhecida pela Arduino IDE, caso deseje migrar para o Arduino IDE será necessário primeiramente alterar a estrutura de pastas do projeto.*

#### IDE

O projeto foi desenvolvido usando o [PlatformIO IDE](https://platformio.org/) no [VS Code](https://code.visualstudio.com/).

1. Instale o `VS Code` [code.visualstudio.com](https://code.visualstudio.com/)
2. Instale a extensão do `PlatformIO IDE` [platformio.org/install/ide?install=vscode](https://platformio.org/install/ide?install=vscode)
3. Na barra esquerda do `VS Code` irá aparecer o icone do `PlatformIO IDE`, clique nele.
4. Dentro do `PlatformIO`, acesse `Platforms` e instale a plataforma `Espressif 8266` [platformio.org/platforms/espressif8266](https://platformio.org/platforms/espressif8266)

#### Abrindo o projeto

1. Baixe o ZIP do projeto no GitHub e extraia em alguma pasta.
2. Na barra esquerda do `VS Code` clique no icone do `PlatformIO IDE`
3. Abra a Home do `PlatformIO`, pressione em `Open Project` e selecione a pasta onde o projeto foi extraido.
4. O arquivo principal do projeto é o `src/main.cpp`

#### Configurações necessárias

1. Crie um arquivo dentro da pasta `include` chamado `globeSecrets.h`
2. Cole nele o conteudo do arquivo de exemplo `globeSecrets.h.example`
3. Preencha com as credenciais de acesso da sua rede Wi-Fi

#### Compilando o projeto

1. Na barra esquerda do `VS Code` clique no icone do `PlatformIO IDE`
2. Pressione `Build`

#### Carregando o projeto no ESP8266 (1ª Vez)

Passos para carregar o programa pela primeira vez no ESP8266.

1. Para carregar o projeto pela primeira vez no ESP8266 é necessário uma conexão USB. Pois o OTA ainda não está em funcionamento.
2. Para a primeira vez também é necessário comentar a linha `upload_port = ...` do arquivo `platformio.ini`
3. Na barra esquerda do `VS Code` clique no icone do `PlatformIO IDE`, pressione `Upload`
4. Aguarde o processo concluir
5. Na barra esquerda do `VS Code` clique no icone do `PlatformIO IDE`, pressione `Upload File System image`
6. Aguarde o processo concluir

#### Carregando o projeto no ESP8266

Passos para carregar o programa no ESP8266.

1. Certifique-se que a linha `upload_port = ...` do arquivo `platformio.ini` contém o IP do ESP8266
2. Na barra esquerda do `VS Code` clique no icone do `PlatformIO IDE`, pressione `Upload`
3. Aguarde o processo concluir
4. Se houver alguma alteração na pasta data (`File System do ESP8266`) execute o passo 5. Caso contrário não é necessário.
5. Na barra esquerda do `VS Code` clique no icone do `PlatformIO IDE`, pressione `Upload File System image`

---

### Servidor local para desenvolvimento do File System do ESP8266

O File System do ESP8266 é um Website Estático, então podemos executá-lo localmente para desenvolvermos e testarmos.

Python

```sh
# Acesse a pasta contendo o Filesystem do ESP8266
cd data

# Veja a versão do python
python -V

# Se a versão do Python retornada acima for 3.X
python3 -m http.server
# No windows, tente "python" em vez de "python3"

# Se a versão do Python retornada acima for 2.X
python -m SimpleHTTPServer
```

Node.js 12.18.2+ (Server escrito usando esta versão do Node.js, não foi checada a compatibilidade com versões anteriores)

```sh
# nvm use 12.18.2

# Acesse a pasta contendo o servidor web
cd server

# Se for a primeira vez, instale as dependencias do projeto
npm install

# Inicie o servidor
npm start
```

---

### Visualização Online do Website Estático (static web server) carregado no Filesystem do ESP8266

O File System do ESP8266 é um Website Estático, então podemos testa-lo online usando um git raw.

OBS: Você receberá o erro `"Mixed content blocked"` se tentar enviar alguma requisicao para o ESP8266 a partir de uma conexão HTTPS. Para a visualização funcionar, o git raw deve usar HTTP.

[http://ghcdn.rawgit.org/guilhermerodrigues680/globo-terrestre-iot/feature/LittleFS-File-Server-Cesiumjs/data/index.html](http://ghcdn.rawgit.org/guilhermerodrigues680/globo-terrestre-iot/feature/LittleFS-File-Server-Cesiumjs/data/index.html)

[https://raw.githack.com/guilhermerodrigues680/globo-terrestre-iot/feature/LittleFS-File-Server-Cesiumjs/data/index.html](https://raw.githack.com/guilhermerodrigues680/globo-terrestre-iot/feature/LittleFS-File-Server-Cesiumjs/data/index.html)

---

### Sobre o File System do ESP8266 - Website Estático (HTML, CSS e Javascript)

A pasta data do projeto é o Filesystem do ESP8266, sempre que uma requisicao chega, ele verifica se há alguma rota para aquela requisicao, se não houver, ele checa se tem um arquivo com esse destino.

Por exemplo:

- O arquivo `data/about.html` é servido sempre que a rota `/about.html` é chamada, como em `http://192.168.2.108/about.html`

#### Documentação API e WebSockets implementados no ESP8266

A API implementada no ESP8266 foi documentada no padrão OpenAPI 3.0, ela está acessivel na rota `/openapi/index.html` do File System do ESP8266

O WebSocket implementado no ESP8266 foi documentado no padrão AsyncAPI 2.0, ela está acessivel na rota `/asyncapi/index.html` do File System do ESP8266

---

### Sobre o Firmware do ESP8266 (C++)

O ...

---

### TODO

 - [ ] Implementar PWA no Globo Terrestre IoT
 - [ ] Desligar Motor após movimentar
 - [ ] Rota para desligar o sistema
 - [ ] Atualizar documentação

---

### Comandos úteis

```sh
platformio device list --mdns
platformio run --target uploadfs
```

---

### Autor

<table>
  <tr>
    <td align="center"><a href="https://github.com/guilhermerodrigues680"><img src="https://avatars.githubusercontent.com/u/31080853?v=3" width="100px;" alt=""/><br /><sub><b>Guilherme Rodrigues</b></sub></a><br />
  </tr>
</table>

---

### Attribution

<div>Icons made by <a href="https://www.flaticon.com/authors/freepik" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a></div>
