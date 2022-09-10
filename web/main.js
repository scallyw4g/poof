import {EditorView, basicSetup} from "codemirror"
import {javascript} from "@codemirror/lang-javascript"

var TestFunc = null
Module.onRuntimeInitialized = () => {

  TestFunc = Module.cwrap('TestFunc', 'string', ['string', 'number']);
  let arg = 'foobeardfa';
  console.log("js TestFunc() = ", TestFunc(arg, arg.length) );


  let editor = new EditorView({
    extensions: [basicSetup, javascript()],
    parent: document.body
  })

}

