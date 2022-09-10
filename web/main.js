
var TestFunc = null
Module.onRuntimeInitialized = () => {
  TestFunc = Module.cwrap('TestFunc', 'string', ['string', 'number']);
  let arg = 'foobeardfa';
  console.log("js TestFunc() = ", TestFunc(arg, arg.length) );
}

