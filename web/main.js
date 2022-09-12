import {EditorView, basicSetup} from "codemirror"
import {cpp} from "@codemirror/lang-cpp"

var EverythingInitialized = false;

InitExamplesPage = function (filename)
{
  if (EverythingInitialized)
  {
    let editorDiv = document.querySelector("div#editor");
    let outputDiv = document.querySelector("div#output");
    let bigRedButton = document.querySelector("div#big-red-button");

    let DoPoofForWeb = Module.cwrap('DoPoofForWeb', 'string', ['string', 'number']);

    let ex02Src = FS.readFile(filename, {encoding: 'utf8'});

    try
    {
      FS.mkdir('generated');
    }
    catch (e)
    {
    }

    let srcEditor = new EditorView({
      extensions: [basicSetup, cpp()],
      doc: ex02Src,
      parent: editorDiv,
    })

    let outputEditor = new EditorView({
      extensions: [basicSetup, cpp()],
      doc: "// Hit the big red button to generate code!",
      parent: outputDiv,
    })


    bigRedButton.addEventListener('click', () => {

      let srcDoc = srcEditor.state.doc;
      let srcToPoof = srcDoc.sliceString(0, srcDoc.length);

      // console.log(doc);
      // console.log(srcToPoof);


      let ex02Output = DoPoofForWeb(srcToPoof, srcToPoof.length);

      console.log("DoPoofForWeb() {");
      console.log( ex02Output );
      console.log("}");

      let outputDoc = outputEditor.state.doc;
      let updateMessage = {changes: [{from: 0, to: outputDoc.length}, {from: 0, insert: ex02Output}]};

      console.log("updateMessage", updateMessage);
      let transaction = outputEditor.state.update(updateMessage);
      outputEditor.dispatch(transaction);
    });
  }
  else
  {
    console.error('tried to initialize poof view before libraries were all initialized.');
  }

}

window.addEventListener('DOMContentLoaded', () => {
  Module.onRuntimeInitialized = () => {
    EverythingInitialized = true;
  }
});

