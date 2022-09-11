import {EditorView, basicSetup} from "codemirror"
import {cpp} from "@codemirror/lang-cpp"


window.addEventListener('DOMContentLoaded', () => {

  Module.onRuntimeInitialized = () => {

    let editorDiv = document.querySelector("div#editor");
    let outputDiv = document.querySelector("div#output");
    let bigRedButton = document.querySelector("div#big-red-button");

    let DoPoofForWeb = Module.cwrap('DoPoofForWeb', 'number', ['string', 'number']);

    let ex02Src = FS.readFile('examples/002_named_function_syntax/main.c', {encoding: 'utf8'});

    FS.mkdir('generated');

    let srcEditor = new EditorView({
      extensions: [basicSetup, cpp()],
      doc: ex02Src,
      parent: editorDiv,
    })

    bigRedButton.addEventListener('click', () => {

      let doc = srcEditor.state.doc;
      let srcToPoof = doc.sliceString(0, doc.length);

      // console.log(doc);
      // console.log(srcToPoof);

      console.log("DoPoofForWeb()");
      console.log( DoPoofForWeb(srcToPoof, srcToPoof.length) );

      let ex02Output = FS.readFile('generated/metaprogram_another_struct_my_struct.h', {encoding: 'utf8'});

      let outputEditor = new EditorView({
        extensions: [basicSetup, cpp()],
        doc: ex02Output,
        parent: outputDiv,
      })

    });

  }

});

