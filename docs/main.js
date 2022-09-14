import {EditorView, basicSetup, minimalSetup} from "codemirror"
import {cpp} from "@codemirror/lang-cpp"

var defaultOutputEditorText = "//\n// Hit the metaprogramming button to generate code!\n//";

var EnvInitialized = false;
var srcEditor = null;
var outputEditor = null;
var DoPoofForWeb = null;

function InitEditor(message, container)
{
  let Result = new EditorView({
    extensions: [minimalSetup, cpp()],
    doc: message,
    parent: container,
  })
  return Result;
}

function InitEditors(filename)
{
  console.assert(EnvInitialized);

  if (global_EditorsInitialized == false)
  {
    try
    {
      FS.mkdir('generated');
    }
    catch (e)
    {
    }

    let editorDiv = document.querySelector("div#src-editor");
    let outputDiv = document.querySelector("div#output-editor");
    let bigRedButton = document.querySelector("div#metaprogramming-button");
    DoPoofForWeb = Module.cwrap('DoPoofForWeb', 'string', ['string', 'number']);

    srcEditor = InitEditor("// Error loading source ..", editorDiv);
    outputEditor = InitEditor(defaultOutputEditorText, outputDiv);

    bigRedButton.addEventListener('click', () => {

      let srcDoc = srcEditor.state.doc;
      let srcToPoof = srcDoc.sliceString(0, srcDoc.length);

      // console.log(doc);
      // console.log(srcToPoof);

      let poofOutput = DoPoofForWeb(srcToPoof, srcToPoof.length);

      // console.log("DoPoofForWeb() {");
      // console.log( poofOutput );
      // console.log("}");

      let outputDoc = outputEditor.state.doc;
      let updateMessage = {changes: [{from: 0, to: outputDoc.length}, {from: 0, insert: poofOutput}]};

      console.log("updateMessage", updateMessage);
      let transaction = outputEditor.state.update(updateMessage);
      outputEditor.dispatch(transaction);
    });

    global_EditorsInitialized = true;
  }
}

function setEditorText(editor, text)
{
  let outputDoc = editor.state.doc;
  let updateMessage = {changes: [{from: 0, to: outputDoc.length}, {from: 0, insert: text}]};
  let transaction = editor.state.update(updateMessage);
  editor.dispatch(transaction);
}

global_InitExample = function (filename)
{
  console.log(`Initializing example for ${filename}`);

  if (EnvInitialized)
  {
    if (global_EditorsInitialized == false) { InitEditors(); }

    let exSourceText = FS.readFile(filename, {encoding: 'utf8'});

    setEditorText(srcEditor, exSourceText);
    setEditorText(outputEditor, defaultOutputEditorText);
  }
  else
  {
    console.error('tried to initialize poof view before libraries were all initialized.');
  }

}

window.addEventListener('DOMContentLoaded', () => {
  Module.onRuntimeInitialized = () => {
    EnvInitialized = true;
  }
});

