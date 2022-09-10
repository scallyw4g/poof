import {EditorView, basicSetup} from "codemirror"
import {cpp} from "@codemirror/lang-cpp"


window.addEventListener('DOMContentLoaded', () => {

  Module.onRuntimeInitialized = () => {

    let DoPoofForWeb = Module.cwrap('DoPoofForWeb', 'number', ['string', 'number']);

    let ex02 = FS.readFile('examples/002_named_function_syntax/main.c', {encoding: 'utf8'});

    console.log("DoPoofForWeb()");
    console.log( DoPoofForWeb(ex02, ex02.length) );


    let ex02Output = FS.readFile('metaprogram_another_struct_my_struct.h', {encoding: 'utf8'});

    let editorDiv = document.querySelector("div#editor");
    let outputDiv = document.querySelector("div#output");

    let srcEditor = new EditorView({
      extensions: [basicSetup, cpp()],
      doc: ex02,
      parent: editorDiv,
    })

    let outputEditor = new EditorView({
      extensions: [basicSetup, cpp()],
      doc: ex02Output,
      parent: outputDiv,
    })

  }

});

