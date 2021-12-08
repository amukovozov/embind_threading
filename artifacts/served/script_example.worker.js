console.log('start script_example.worker.js');

const n = 30;

console.log('n is set to ' + n);

var processor;

var Module = {
  mainScriptUrlOrBlob: 'example_wasm.js',

  print: function(text) {
    self.postMessage({
      'type': 'print',
      'text': text
    });
  },

  onRuntimeInitialized: function() {
    console.log('===  start onRuntimeInitialized  ===');

    processor = new Module.Processor();
    console.log('===  processor created  ===');

    self.postMessage({
      'type': 'worker_ready'
    });

    console.log('===  end onRuntimeInitialized  ===');
  }
};

self.addEventListener('message', (msg) => {
  if (msg.data.type === 'click') {
    console.log('Process(' + msg.data.id + ')');
    switch (msg.data.id) {
      case 'no_threads':
        var result = processor.ProcessNoThreads(n);
        self.postMessage({ 'type': 'done', 'result': result });
        break;
      case 'threads_flat':
        var result = processor.ProcessThreadsFlat(n);
        self.postMessage({ 'type': 'done', 'result': result });
        break;
      case 'threads_nested':
        var result = processor.ProcessThreadsNested(n);
        self.postMessage({ 'type': 'done', 'result': result });
        break;
      case 'tg_flat':
        var result = processor.ProcessTGFlat(n);
        self.postMessage({ 'type': 'done', 'result': result });
        break;
      case 'tg_nested':
        var result = processor.ProcessTGNested(n);
        self.postMessage({ 'type': 'done', 'result': result });
        break;
    }
  }
});

importScripts('example_wasm.js');
