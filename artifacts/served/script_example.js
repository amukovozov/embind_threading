"use strict";

console.log('start script_example.js');

var worker = new Worker('script_example.worker.js');

worker.addEventListener('error', (err) => {
  console.log('ERROR FROM WORKER: ' + err.data);
});

worker.addEventListener('message', (msg) => {
  if (msg.data.type === 'worker_ready') {
    div_buttons.hidden = false;
  } else if (msg.data.type === 'done') {
    console.log('result: ' + msg.data.result);
  } else if (msg.data.type === 'print') {
    console.log('PRINT FROM MODULE: ' + msg.data.text);
  } else {
    console.log('UNEXPECTED MESSAGE FROM WORKER: ' + msg);
  }
});

button_no_threads.addEventListener('click', (e) => {
  worker.postMessage({ 'type': 'click', 'id': 'no_threads' });
});
button_threads_flat.addEventListener('click', (e) => {
  worker.postMessage({ 'type': 'click', 'id': 'threads_flat' });
});
button_threads_nested.addEventListener('click', (e) => {
  worker.postMessage({ 'type': 'click', 'id': 'threads_nested' });
});
