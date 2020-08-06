Crie o arquivo `src/arduino_secrets.h` seguindo o modelo do `src/arduino_secrets.h.example`

`platformio device list --mdns`

`platformio run --target uploadfs`


https://cdn.jsdelivr.net/gh/guilhermerodrigues680/esp8266-globo-terrestre/HTML-Pages/404.html
https://raw.githack.com/guilhermerodrigues680/esp8266-globo-terrestre/master/HTML-Pages/404.html


### Servidor de testes local

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
# Acesse a pasta contendo o servidor web
cd server

# Se for a primeira vez, instale as dependencias do projeto
npm install

# Node.js
nvm use 12.18.2
npm start
```
