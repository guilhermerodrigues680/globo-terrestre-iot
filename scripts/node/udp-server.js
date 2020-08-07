// -------------------- Servidor UDP --------------------
const udp = require('dgram');
const ip = require('ip');

const PORT = 2222;
const server = udp.createSocket('udp4'); // creating a udp server

process.on('SIGINT', () => {
  console.log("\nDesligando servidor UDP...");
  server.close(() => {
    process.exit();
  });
});

// emits when any error occurs
server.on('error', (error) => {
  console.log('Error: ' + error);
  server.close();
});

// emits on new datagram msg
server.on('message', (msg, rinfo) => {
  const date = new Date().toLocaleString('pt-br');
  const rAddr = rinfo.address;
  const rPort = rinfo.port;
  const ipAddr = ip.address();
  const size = msg.length;
  const msgStr = JSON.stringify(msg.toString());

  console.log(`${date} [${rAddr}:${rPort}]->[${ipAddr}:${PORT}] (${size} bytes): ${msgStr}`);

  //sending msg
  // server.send(msg, rinfo.port, ip.address(), (error) => {
  //   if (error) {
  //     console.error(error);
  //   } else {
  //     console.log('Data sent !!!');
  //   }
  // });
});

//emits when socket is ready and listening for datagram msgs
server.on('listening', () => {
  const address = server.address();
  console.log('\n---------------------------------\n');
  console.log(`Server: Porta: ${address.port}`);
  console.log(`Server: IP Local: ${ip.address()}`);
  console.log(`Server: IP: ${address.address}`);
  console.log(`Server: é IP4/IP6: ${address.family}`);
  console.log('\n---------------------------------\n');
});

//emits after the socket is closed using socket.close();
server.on('close', function () {
  console.log('Socket fechado!');
});

server.bind(PORT);
