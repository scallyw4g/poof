(function () {
   'use strict';

   /**
   The data structure for documents. @nonabstract
   */
   class Text {
       /**
       @internal
       */
       constructor() { }
       /**
       Get the line description around the given position.
       */
       lineAt(pos) {
           if (pos < 0 || pos > this.length)
               throw new RangeError(`Invalid position ${pos} in document of length ${this.length}`);
           return this.lineInner(pos, false, 1, 0);
       }
       /**
       Get the description for the given (1-based) line number.
       */
       line(n) {
           if (n < 1 || n > this.lines)
               throw new RangeError(`Invalid line number ${n} in ${this.lines}-line document`);
           return this.lineInner(n, true, 1, 0);
       }
       /**
       Replace a range of the text with the given content.
       */
       replace(from, to, text) {
           let parts = [];
           this.decompose(0, from, parts, 2 /* To */);
           if (text.length)
               text.decompose(0, text.length, parts, 1 /* From */ | 2 /* To */);
           this.decompose(to, this.length, parts, 1 /* From */);
           return TextNode.from(parts, this.length - (to - from) + text.length);
       }
       /**
       Append another document to this one.
       */
       append(other) {
           return this.replace(this.length, this.length, other);
       }
       /**
       Retrieve the text between the given points.
       */
       slice(from, to = this.length) {
           let parts = [];
           this.decompose(from, to, parts, 0);
           return TextNode.from(parts, to - from);
       }
       /**
       Test whether this text is equal to another instance.
       */
       eq(other) {
           if (other == this)
               return true;
           if (other.length != this.length || other.lines != this.lines)
               return false;
           let start = this.scanIdentical(other, 1), end = this.length - this.scanIdentical(other, -1);
           let a = new RawTextCursor(this), b = new RawTextCursor(other);
           for (let skip = start, pos = start;;) {
               a.next(skip);
               b.next(skip);
               skip = 0;
               if (a.lineBreak != b.lineBreak || a.done != b.done || a.value != b.value)
                   return false;
               pos += a.value.length;
               if (a.done || pos >= end)
                   return true;
           }
       }
       /**
       Iterate over the text. When `dir` is `-1`, iteration happens
       from end to start. This will return lines and the breaks between
       them as separate strings.
       */
       iter(dir = 1) { return new RawTextCursor(this, dir); }
       /**
       Iterate over a range of the text. When `from` > `to`, the
       iterator will run in reverse.
       */
       iterRange(from, to = this.length) { return new PartialTextCursor(this, from, to); }
       /**
       Return a cursor that iterates over the given range of lines,
       _without_ returning the line breaks between, and yielding empty
       strings for empty lines.
       
       When `from` and `to` are given, they should be 1-based line numbers.
       */
       iterLines(from, to) {
           let inner;
           if (from == null) {
               inner = this.iter();
           }
           else {
               if (to == null)
                   to = this.lines + 1;
               let start = this.line(from).from;
               inner = this.iterRange(start, Math.max(start, to == this.lines + 1 ? this.length : to <= 1 ? 0 : this.line(to - 1).to));
           }
           return new LineCursor(inner);
       }
       /**
       @internal
       */
       toString() { return this.sliceString(0); }
       /**
       Convert the document to an array of lines (which can be
       deserialized again via [`Text.of`](https://codemirror.net/6/docs/ref/#state.Text^of)).
       */
       toJSON() {
           let lines = [];
           this.flatten(lines);
           return lines;
       }
       /**
       Create a `Text` instance for the given array of lines.
       */
       static of(text) {
           if (text.length == 0)
               throw new RangeError("A document must have at least one line");
           if (text.length == 1 && !text[0])
               return Text.empty;
           return text.length <= 32 /* Branch */ ? new TextLeaf(text) : TextNode.from(TextLeaf.split(text, []));
       }
   }
   // Leaves store an array of line strings. There are always line breaks
   // between these strings. Leaves are limited in size and have to be
   // contained in TextNode instances for bigger documents.
   class TextLeaf extends Text {
       constructor(text, length = textLength(text)) {
           super();
           this.text = text;
           this.length = length;
       }
       get lines() { return this.text.length; }
       get children() { return null; }
       lineInner(target, isLine, line, offset) {
           for (let i = 0;; i++) {
               let string = this.text[i], end = offset + string.length;
               if ((isLine ? line : end) >= target)
                   return new Line(offset, end, line, string);
               offset = end + 1;
               line++;
           }
       }
       decompose(from, to, target, open) {
           let text = from <= 0 && to >= this.length ? this
               : new TextLeaf(sliceText(this.text, from, to), Math.min(to, this.length) - Math.max(0, from));
           if (open & 1 /* From */) {
               let prev = target.pop();
               let joined = appendText(text.text, prev.text.slice(), 0, text.length);
               if (joined.length <= 32 /* Branch */) {
                   target.push(new TextLeaf(joined, prev.length + text.length));
               }
               else {
                   let mid = joined.length >> 1;
                   target.push(new TextLeaf(joined.slice(0, mid)), new TextLeaf(joined.slice(mid)));
               }
           }
           else {
               target.push(text);
           }
       }
       replace(from, to, text) {
           if (!(text instanceof TextLeaf))
               return super.replace(from, to, text);
           let lines = appendText(this.text, appendText(text.text, sliceText(this.text, 0, from)), to);
           let newLen = this.length + text.length - (to - from);
           if (lines.length <= 32 /* Branch */)
               return new TextLeaf(lines, newLen);
           return TextNode.from(TextLeaf.split(lines, []), newLen);
       }
       sliceString(from, to = this.length, lineSep = "\n") {
           let result = "";
           for (let pos = 0, i = 0; pos <= to && i < this.text.length; i++) {
               let line = this.text[i], end = pos + line.length;
               if (pos > from && i)
                   result += lineSep;
               if (from < end && to > pos)
                   result += line.slice(Math.max(0, from - pos), to - pos);
               pos = end + 1;
           }
           return result;
       }
       flatten(target) {
           for (let line of this.text)
               target.push(line);
       }
       scanIdentical() { return 0; }
       static split(text, target) {
           let part = [], len = -1;
           for (let line of text) {
               part.push(line);
               len += line.length + 1;
               if (part.length == 32 /* Branch */) {
                   target.push(new TextLeaf(part, len));
                   part = [];
                   len = -1;
               }
           }
           if (len > -1)
               target.push(new TextLeaf(part, len));
           return target;
       }
   }
   // Nodes provide the tree structure of the `Text` type. They store a
   // number of other nodes or leaves, taking care to balance themselves
   // on changes. There are implied line breaks _between_ the children of
   // a node (but not before the first or after the last child).
   class TextNode extends Text {
       constructor(children, length) {
           super();
           this.children = children;
           this.length = length;
           this.lines = 0;
           for (let child of children)
               this.lines += child.lines;
       }
       lineInner(target, isLine, line, offset) {
           for (let i = 0;; i++) {
               let child = this.children[i], end = offset + child.length, endLine = line + child.lines - 1;
               if ((isLine ? endLine : end) >= target)
                   return child.lineInner(target, isLine, line, offset);
               offset = end + 1;
               line = endLine + 1;
           }
       }
       decompose(from, to, target, open) {
           for (let i = 0, pos = 0; pos <= to && i < this.children.length; i++) {
               let child = this.children[i], end = pos + child.length;
               if (from <= end && to >= pos) {
                   let childOpen = open & ((pos <= from ? 1 /* From */ : 0) | (end >= to ? 2 /* To */ : 0));
                   if (pos >= from && end <= to && !childOpen)
                       target.push(child);
                   else
                       child.decompose(from - pos, to - pos, target, childOpen);
               }
               pos = end + 1;
           }
       }
       replace(from, to, text) {
           if (text.lines < this.lines)
               for (let i = 0, pos = 0; i < this.children.length; i++) {
                   let child = this.children[i], end = pos + child.length;
                   // Fast path: if the change only affects one child and the
                   // child's size remains in the acceptable range, only update
                   // that child
                   if (from >= pos && to <= end) {
                       let updated = child.replace(from - pos, to - pos, text);
                       let totalLines = this.lines - child.lines + updated.lines;
                       if (updated.lines < (totalLines >> (5 /* BranchShift */ - 1)) &&
                           updated.lines > (totalLines >> (5 /* BranchShift */ + 1))) {
                           let copy = this.children.slice();
                           copy[i] = updated;
                           return new TextNode(copy, this.length - (to - from) + text.length);
                       }
                       return super.replace(pos, end, updated);
                   }
                   pos = end + 1;
               }
           return super.replace(from, to, text);
       }
       sliceString(from, to = this.length, lineSep = "\n") {
           let result = "";
           for (let i = 0, pos = 0; i < this.children.length && pos <= to; i++) {
               let child = this.children[i], end = pos + child.length;
               if (pos > from && i)
                   result += lineSep;
               if (from < end && to > pos)
                   result += child.sliceString(from - pos, to - pos, lineSep);
               pos = end + 1;
           }
           return result;
       }
       flatten(target) {
           for (let child of this.children)
               child.flatten(target);
       }
       scanIdentical(other, dir) {
           if (!(other instanceof TextNode))
               return 0;
           let length = 0;
           let [iA, iB, eA, eB] = dir > 0 ? [0, 0, this.children.length, other.children.length]
               : [this.children.length - 1, other.children.length - 1, -1, -1];
           for (;; iA += dir, iB += dir) {
               if (iA == eA || iB == eB)
                   return length;
               let chA = this.children[iA], chB = other.children[iB];
               if (chA != chB)
                   return length + chA.scanIdentical(chB, dir);
               length += chA.length + 1;
           }
       }
       static from(children, length = children.reduce((l, ch) => l + ch.length + 1, -1)) {
           let lines = 0;
           for (let ch of children)
               lines += ch.lines;
           if (lines < 32 /* Branch */) {
               let flat = [];
               for (let ch of children)
                   ch.flatten(flat);
               return new TextLeaf(flat, length);
           }
           let chunk = Math.max(32 /* Branch */, lines >> 5 /* BranchShift */), maxChunk = chunk << 1, minChunk = chunk >> 1;
           let chunked = [], currentLines = 0, currentLen = -1, currentChunk = [];
           function add(child) {
               let last;
               if (child.lines > maxChunk && child instanceof TextNode) {
                   for (let node of child.children)
                       add(node);
               }
               else if (child.lines > minChunk && (currentLines > minChunk || !currentLines)) {
                   flush();
                   chunked.push(child);
               }
               else if (child instanceof TextLeaf && currentLines &&
                   (last = currentChunk[currentChunk.length - 1]) instanceof TextLeaf &&
                   child.lines + last.lines <= 32 /* Branch */) {
                   currentLines += child.lines;
                   currentLen += child.length + 1;
                   currentChunk[currentChunk.length - 1] = new TextLeaf(last.text.concat(child.text), last.length + 1 + child.length);
               }
               else {
                   if (currentLines + child.lines > chunk)
                       flush();
                   currentLines += child.lines;
                   currentLen += child.length + 1;
                   currentChunk.push(child);
               }
           }
           function flush() {
               if (currentLines == 0)
                   return;
               chunked.push(currentChunk.length == 1 ? currentChunk[0] : TextNode.from(currentChunk, currentLen));
               currentLen = -1;
               currentLines = currentChunk.length = 0;
           }
           for (let child of children)
               add(child);
           flush();
           return chunked.length == 1 ? chunked[0] : new TextNode(chunked, length);
       }
   }
   Text.empty = /*@__PURE__*/new TextLeaf([""], 0);
   function textLength(text) {
       let length = -1;
       for (let line of text)
           length += line.length + 1;
       return length;
   }
   function appendText(text, target, from = 0, to = 1e9) {
       for (let pos = 0, i = 0, first = true; i < text.length && pos <= to; i++) {
           let line = text[i], end = pos + line.length;
           if (end >= from) {
               if (end > to)
                   line = line.slice(0, to - pos);
               if (pos < from)
                   line = line.slice(from - pos);
               if (first) {
                   target[target.length - 1] += line;
                   first = false;
               }
               else
                   target.push(line);
           }
           pos = end + 1;
       }
       return target;
   }
   function sliceText(text, from, to) {
       return appendText(text, [""], from, to);
   }
   class RawTextCursor {
       constructor(text, dir = 1) {
           this.dir = dir;
           this.done = false;
           this.lineBreak = false;
           this.value = "";
           this.nodes = [text];
           this.offsets = [dir > 0 ? 1 : (text instanceof TextLeaf ? text.text.length : text.children.length) << 1];
       }
       nextInner(skip, dir) {
           this.done = this.lineBreak = false;
           for (;;) {
               let last = this.nodes.length - 1;
               let top = this.nodes[last], offsetValue = this.offsets[last], offset = offsetValue >> 1;
               let size = top instanceof TextLeaf ? top.text.length : top.children.length;
               if (offset == (dir > 0 ? size : 0)) {
                   if (last == 0) {
                       this.done = true;
                       this.value = "";
                       return this;
                   }
                   if (dir > 0)
                       this.offsets[last - 1]++;
                   this.nodes.pop();
                   this.offsets.pop();
               }
               else if ((offsetValue & 1) == (dir > 0 ? 0 : 1)) {
                   this.offsets[last] += dir;
                   if (skip == 0) {
                       this.lineBreak = true;
                       this.value = "\n";
                       return this;
                   }
                   skip--;
               }
               else if (top instanceof TextLeaf) {
                   // Move to the next string
                   let next = top.text[offset + (dir < 0 ? -1 : 0)];
                   this.offsets[last] += dir;
                   if (next.length > Math.max(0, skip)) {
                       this.value = skip == 0 ? next : dir > 0 ? next.slice(skip) : next.slice(0, next.length - skip);
                       return this;
                   }
                   skip -= next.length;
               }
               else {
                   let next = top.children[offset + (dir < 0 ? -1 : 0)];
                   if (skip > next.length) {
                       skip -= next.length;
                       this.offsets[last] += dir;
                   }
                   else {
                       if (dir < 0)
                           this.offsets[last]--;
                       this.nodes.push(next);
                       this.offsets.push(dir > 0 ? 1 : (next instanceof TextLeaf ? next.text.length : next.children.length) << 1);
                   }
               }
           }
       }
       next(skip = 0) {
           if (skip < 0) {
               this.nextInner(-skip, (-this.dir));
               skip = this.value.length;
           }
           return this.nextInner(skip, this.dir);
       }
   }
   class PartialTextCursor {
       constructor(text, start, end) {
           this.value = "";
           this.done = false;
           this.cursor = new RawTextCursor(text, start > end ? -1 : 1);
           this.pos = start > end ? text.length : 0;
           this.from = Math.min(start, end);
           this.to = Math.max(start, end);
       }
       nextInner(skip, dir) {
           if (dir < 0 ? this.pos <= this.from : this.pos >= this.to) {
               this.value = "";
               this.done = true;
               return this;
           }
           skip += Math.max(0, dir < 0 ? this.pos - this.to : this.from - this.pos);
           let limit = dir < 0 ? this.pos - this.from : this.to - this.pos;
           if (skip > limit)
               skip = limit;
           limit -= skip;
           let { value } = this.cursor.next(skip);
           this.pos += (value.length + skip) * dir;
           this.value = value.length <= limit ? value : dir < 0 ? value.slice(value.length - limit) : value.slice(0, limit);
           this.done = !this.value;
           return this;
       }
       next(skip = 0) {
           if (skip < 0)
               skip = Math.max(skip, this.from - this.pos);
           else if (skip > 0)
               skip = Math.min(skip, this.to - this.pos);
           return this.nextInner(skip, this.cursor.dir);
       }
       get lineBreak() { return this.cursor.lineBreak && this.value != ""; }
   }
   class LineCursor {
       constructor(inner) {
           this.inner = inner;
           this.afterBreak = true;
           this.value = "";
           this.done = false;
       }
       next(skip = 0) {
           let { done, lineBreak, value } = this.inner.next(skip);
           if (done) {
               this.done = true;
               this.value = "";
           }
           else if (lineBreak) {
               if (this.afterBreak) {
                   this.value = "";
               }
               else {
                   this.afterBreak = true;
                   this.next();
               }
           }
           else {
               this.value = value;
               this.afterBreak = false;
           }
           return this;
       }
       get lineBreak() { return false; }
   }
   if (typeof Symbol != "undefined") {
       Text.prototype[Symbol.iterator] = function () { return this.iter(); };
       RawTextCursor.prototype[Symbol.iterator] = PartialTextCursor.prototype[Symbol.iterator] =
           LineCursor.prototype[Symbol.iterator] = function () { return this; };
   }
   /**
   This type describes a line in the document. It is created
   on-demand when lines are [queried](https://codemirror.net/6/docs/ref/#state.Text.lineAt).
   */
   class Line {
       /**
       @internal
       */
       constructor(
       /**
       The position of the start of the line.
       */
       from, 
       /**
       The position at the end of the line (_before_ the line break,
       or at the end of document for the last line).
       */
       to, 
       /**
       This line's line number (1-based).
       */
       number, 
       /**
       The line's content.
       */
       text) {
           this.from = from;
           this.to = to;
           this.number = number;
           this.text = text;
       }
       /**
       The length of the line (not including any line break after it).
       */
       get length() { return this.to - this.from; }
   }

   // Compressed representation of the Grapheme_Cluster_Break=Extend
   // information from
   // http://www.unicode.org/Public/13.0.0/ucd/auxiliary/GraphemeBreakProperty.txt.
   // Each pair of elements represents a range, as an offet from the
   // previous range and a length. Numbers are in base-36, with the empty
   // string being a shorthand for 1.
   let extend = /*@__PURE__*/"lc,34,7n,7,7b,19,,,,2,,2,,,20,b,1c,l,g,,2t,7,2,6,2,2,,4,z,,u,r,2j,b,1m,9,9,,o,4,,9,,3,,5,17,3,3b,f,,w,1j,,,,4,8,4,,3,7,a,2,t,,1m,,,,2,4,8,,9,,a,2,q,,2,2,1l,,4,2,4,2,2,3,3,,u,2,3,,b,2,1l,,4,5,,2,4,,k,2,m,6,,,1m,,,2,,4,8,,7,3,a,2,u,,1n,,,,c,,9,,14,,3,,1l,3,5,3,,4,7,2,b,2,t,,1m,,2,,2,,3,,5,2,7,2,b,2,s,2,1l,2,,,2,4,8,,9,,a,2,t,,20,,4,,2,3,,,8,,29,,2,7,c,8,2q,,2,9,b,6,22,2,r,,,,,,1j,e,,5,,2,5,b,,10,9,,2u,4,,6,,2,2,2,p,2,4,3,g,4,d,,2,2,6,,f,,jj,3,qa,3,t,3,t,2,u,2,1s,2,,7,8,,2,b,9,,19,3,3b,2,y,,3a,3,4,2,9,,6,3,63,2,2,,1m,,,7,,,,,2,8,6,a,2,,1c,h,1r,4,1c,7,,,5,,14,9,c,2,w,4,2,2,,3,1k,,,2,3,,,3,1m,8,2,2,48,3,,d,,7,4,,6,,3,2,5i,1m,,5,ek,,5f,x,2da,3,3x,,2o,w,fe,6,2x,2,n9w,4,,a,w,2,28,2,7k,,3,,4,,p,2,5,,47,2,q,i,d,,12,8,p,b,1a,3,1c,,2,4,2,2,13,,1v,6,2,2,2,2,c,,8,,1b,,1f,,,3,2,2,5,2,,,16,2,8,,6m,,2,,4,,fn4,,kh,g,g,g,a6,2,gt,,6a,,45,5,1ae,3,,2,5,4,14,3,4,,4l,2,fx,4,ar,2,49,b,4w,,1i,f,1k,3,1d,4,2,2,1x,3,10,5,,8,1q,,c,2,1g,9,a,4,2,,2n,3,2,,,2,6,,4g,,3,8,l,2,1l,2,,,,,m,,e,7,3,5,5f,8,2,3,,,n,,29,,2,6,,,2,,,2,,2,6j,,2,4,6,2,,2,r,2,2d,8,2,,,2,2y,,,,2,6,,,2t,3,2,4,,5,77,9,,2,6t,,a,2,,,4,,40,4,2,2,4,,w,a,14,6,2,4,8,,9,6,2,3,1a,d,,2,ba,7,,6,,,2a,m,2,7,,2,,2,3e,6,3,,,2,,7,,,20,2,3,,,,9n,2,f0b,5,1n,7,t4,,1r,4,29,,f5k,2,43q,,,3,4,5,8,8,2,7,u,4,44,3,1iz,1j,4,1e,8,,e,,m,5,,f,11s,7,,h,2,7,,2,,5,79,7,c5,4,15s,7,31,7,240,5,gx7k,2o,3k,6o".split(",").map(s => s ? parseInt(s, 36) : 1);
   // Convert offsets into absolute values
   for (let i = 1; i < extend.length; i++)
       extend[i] += extend[i - 1];
   function isExtendingChar(code) {
       for (let i = 1; i < extend.length; i += 2)
           if (extend[i] > code)
               return extend[i - 1] <= code;
       return false;
   }
   function isRegionalIndicator(code) {
       return code >= 0x1F1E6 && code <= 0x1F1FF;
   }
   const ZWJ = 0x200d;
   /**
   Returns a next grapheme cluster break _after_ (not equal to)
   `pos`, if `forward` is true, or before otherwise. Returns `pos`
   itself if no further cluster break is available in the string.
   Moves across surrogate pairs, extending characters (when
   `includeExtending` is true), characters joined with zero-width
   joiners, and flag emoji.
   */
   function findClusterBreak(str, pos, forward = true, includeExtending = true) {
       return (forward ? nextClusterBreak : prevClusterBreak)(str, pos, includeExtending);
   }
   function nextClusterBreak(str, pos, includeExtending) {
       if (pos == str.length)
           return pos;
       // If pos is in the middle of a surrogate pair, move to its start
       if (pos && surrogateLow(str.charCodeAt(pos)) && surrogateHigh(str.charCodeAt(pos - 1)))
           pos--;
       let prev = codePointAt(str, pos);
       pos += codePointSize(prev);
       while (pos < str.length) {
           let next = codePointAt(str, pos);
           if (prev == ZWJ || next == ZWJ || includeExtending && isExtendingChar(next)) {
               pos += codePointSize(next);
               prev = next;
           }
           else if (isRegionalIndicator(next)) {
               let countBefore = 0, i = pos - 2;
               while (i >= 0 && isRegionalIndicator(codePointAt(str, i))) {
                   countBefore++;
                   i -= 2;
               }
               if (countBefore % 2 == 0)
                   break;
               else
                   pos += 2;
           }
           else {
               break;
           }
       }
       return pos;
   }
   function prevClusterBreak(str, pos, includeExtending) {
       while (pos > 0) {
           let found = nextClusterBreak(str, pos - 2, includeExtending);
           if (found < pos)
               return found;
           pos--;
       }
       return 0;
   }
   function surrogateLow(ch) { return ch >= 0xDC00 && ch < 0xE000; }
   function surrogateHigh(ch) { return ch >= 0xD800 && ch < 0xDC00; }
   /**
   Find the code point at the given position in a string (like the
   [`codePointAt`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/codePointAt)
   string method).
   */
   function codePointAt(str, pos) {
       let code0 = str.charCodeAt(pos);
       if (!surrogateHigh(code0) || pos + 1 == str.length)
           return code0;
       let code1 = str.charCodeAt(pos + 1);
       if (!surrogateLow(code1))
           return code0;
       return ((code0 - 0xd800) << 10) + (code1 - 0xdc00) + 0x10000;
   }
   /**
   The amount of positions a character takes up a JavaScript string.
   */
   function codePointSize(code) { return code < 0x10000 ? 1 : 2; }

   const DefaultSplit = /\r\n?|\n/;
   /**
   Distinguishes different ways in which positions can be mapped.
   */
   var MapMode = /*@__PURE__*/(function (MapMode) {
       /**
       Map a position to a valid new position, even when its context
       was deleted.
       */
       MapMode[MapMode["Simple"] = 0] = "Simple";
       /**
       Return null if deletion happens across the position.
       */
       MapMode[MapMode["TrackDel"] = 1] = "TrackDel";
       /**
       Return null if the character _before_ the position is deleted.
       */
       MapMode[MapMode["TrackBefore"] = 2] = "TrackBefore";
       /**
       Return null if the character _after_ the position is deleted.
       */
       MapMode[MapMode["TrackAfter"] = 3] = "TrackAfter";
   return MapMode})(MapMode || (MapMode = {}));
   /**
   A change description is a variant of [change set](https://codemirror.net/6/docs/ref/#state.ChangeSet)
   that doesn't store the inserted text. As such, it can't be
   applied, but is cheaper to store and manipulate.
   */
   class ChangeDesc {
       // Sections are encoded as pairs of integers. The first is the
       // length in the current document, and the second is -1 for
       // unaffected sections, and the length of the replacement content
       // otherwise. So an insertion would be (0, n>0), a deletion (n>0,
       // 0), and a replacement two positive numbers.
       /**
       @internal
       */
       constructor(
       /**
       @internal
       */
       sections) {
           this.sections = sections;
       }
       /**
       The length of the document before the change.
       */
       get length() {
           let result = 0;
           for (let i = 0; i < this.sections.length; i += 2)
               result += this.sections[i];
           return result;
       }
       /**
       The length of the document after the change.
       */
       get newLength() {
           let result = 0;
           for (let i = 0; i < this.sections.length; i += 2) {
               let ins = this.sections[i + 1];
               result += ins < 0 ? this.sections[i] : ins;
           }
           return result;
       }
       /**
       False when there are actual changes in this set.
       */
       get empty() { return this.sections.length == 0 || this.sections.length == 2 && this.sections[1] < 0; }
       /**
       Iterate over the unchanged parts left by these changes. `posA`
       provides the position of the range in the old document, `posB`
       the new position in the changed document.
       */
       iterGaps(f) {
           for (let i = 0, posA = 0, posB = 0; i < this.sections.length;) {
               let len = this.sections[i++], ins = this.sections[i++];
               if (ins < 0) {
                   f(posA, posB, len);
                   posB += len;
               }
               else {
                   posB += ins;
               }
               posA += len;
           }
       }
       /**
       Iterate over the ranges changed by these changes. (See
       [`ChangeSet.iterChanges`](https://codemirror.net/6/docs/ref/#state.ChangeSet.iterChanges) for a
       variant that also provides you with the inserted text.)
       `fromA`/`toA` provides the extent of the change in the starting
       document, `fromB`/`toB` the extent of the replacement in the
       changed document.
       
       When `individual` is true, adjacent changes (which are kept
       separate for [position mapping](https://codemirror.net/6/docs/ref/#state.ChangeDesc.mapPos)) are
       reported separately.
       */
       iterChangedRanges(f, individual = false) {
           iterChanges(this, f, individual);
       }
       /**
       Get a description of the inverted form of these changes.
       */
       get invertedDesc() {
           let sections = [];
           for (let i = 0; i < this.sections.length;) {
               let len = this.sections[i++], ins = this.sections[i++];
               if (ins < 0)
                   sections.push(len, ins);
               else
                   sections.push(ins, len);
           }
           return new ChangeDesc(sections);
       }
       /**
       Compute the combined effect of applying another set of changes
       after this one. The length of the document after this set should
       match the length before `other`.
       */
       composeDesc(other) { return this.empty ? other : other.empty ? this : composeSets(this, other); }
       /**
       Map this description, which should start with the same document
       as `other`, over another set of changes, so that it can be
       applied after it. When `before` is true, map as if the changes
       in `other` happened before the ones in `this`.
       */
       mapDesc(other, before = false) { return other.empty ? this : mapSet(this, other, before); }
       mapPos(pos, assoc = -1, mode = MapMode.Simple) {
           let posA = 0, posB = 0;
           for (let i = 0; i < this.sections.length;) {
               let len = this.sections[i++], ins = this.sections[i++], endA = posA + len;
               if (ins < 0) {
                   if (endA > pos)
                       return posB + (pos - posA);
                   posB += len;
               }
               else {
                   if (mode != MapMode.Simple && endA >= pos &&
                       (mode == MapMode.TrackDel && posA < pos && endA > pos ||
                           mode == MapMode.TrackBefore && posA < pos ||
                           mode == MapMode.TrackAfter && endA > pos))
                       return null;
                   if (endA > pos || endA == pos && assoc < 0 && !len)
                       return pos == posA || assoc < 0 ? posB : posB + ins;
                   posB += ins;
               }
               posA = endA;
           }
           if (pos > posA)
               throw new RangeError(`Position ${pos} is out of range for changeset of length ${posA}`);
           return posB;
       }
       /**
       Check whether these changes touch a given range. When one of the
       changes entirely covers the range, the string `"cover"` is
       returned.
       */
       touchesRange(from, to = from) {
           for (let i = 0, pos = 0; i < this.sections.length && pos <= to;) {
               let len = this.sections[i++], ins = this.sections[i++], end = pos + len;
               if (ins >= 0 && pos <= to && end >= from)
                   return pos < from && end > to ? "cover" : true;
               pos = end;
           }
           return false;
       }
       /**
       @internal
       */
       toString() {
           let result = "";
           for (let i = 0; i < this.sections.length;) {
               let len = this.sections[i++], ins = this.sections[i++];
               result += (result ? " " : "") + len + (ins >= 0 ? ":" + ins : "");
           }
           return result;
       }
       /**
       Serialize this change desc to a JSON-representable value.
       */
       toJSON() { return this.sections; }
       /**
       Create a change desc from its JSON representation (as produced
       by [`toJSON`](https://codemirror.net/6/docs/ref/#state.ChangeDesc.toJSON).
       */
       static fromJSON(json) {
           if (!Array.isArray(json) || json.length % 2 || json.some(a => typeof a != "number"))
               throw new RangeError("Invalid JSON representation of ChangeDesc");
           return new ChangeDesc(json);
       }
       /**
       @internal
       */
       static create(sections) { return new ChangeDesc(sections); }
   }
   /**
   A change set represents a group of modifications to a document. It
   stores the document length, and can only be applied to documents
   with exactly that length.
   */
   class ChangeSet extends ChangeDesc {
       constructor(sections, 
       /**
       @internal
       */
       inserted) {
           super(sections);
           this.inserted = inserted;
       }
       /**
       Apply the changes to a document, returning the modified
       document.
       */
       apply(doc) {
           if (this.length != doc.length)
               throw new RangeError("Applying change set to a document with the wrong length");
           iterChanges(this, (fromA, toA, fromB, _toB, text) => doc = doc.replace(fromB, fromB + (toA - fromA), text), false);
           return doc;
       }
       mapDesc(other, before = false) { return mapSet(this, other, before, true); }
       /**
       Given the document as it existed _before_ the changes, return a
       change set that represents the inverse of this set, which could
       be used to go from the document created by the changes back to
       the document as it existed before the changes.
       */
       invert(doc) {
           let sections = this.sections.slice(), inserted = [];
           for (let i = 0, pos = 0; i < sections.length; i += 2) {
               let len = sections[i], ins = sections[i + 1];
               if (ins >= 0) {
                   sections[i] = ins;
                   sections[i + 1] = len;
                   let index = i >> 1;
                   while (inserted.length < index)
                       inserted.push(Text.empty);
                   inserted.push(len ? doc.slice(pos, pos + len) : Text.empty);
               }
               pos += len;
           }
           return new ChangeSet(sections, inserted);
       }
       /**
       Combine two subsequent change sets into a single set. `other`
       must start in the document produced by `this`. If `this` goes
       `docA` → `docB` and `other` represents `docB` → `docC`, the
       returned value will represent the change `docA` → `docC`.
       */
       compose(other) { return this.empty ? other : other.empty ? this : composeSets(this, other, true); }
       /**
       Given another change set starting in the same document, maps this
       change set over the other, producing a new change set that can be
       applied to the document produced by applying `other`. When
       `before` is `true`, order changes as if `this` comes before
       `other`, otherwise (the default) treat `other` as coming first.
       
       Given two changes `A` and `B`, `A.compose(B.map(A))` and
       `B.compose(A.map(B, true))` will produce the same document. This
       provides a basic form of [operational
       transformation](https://en.wikipedia.org/wiki/Operational_transformation),
       and can be used for collaborative editing.
       */
       map(other, before = false) { return other.empty ? this : mapSet(this, other, before, true); }
       /**
       Iterate over the changed ranges in the document, calling `f` for
       each, with the range in the original document (`fromA`-`toA`)
       and the range that replaces it in the new document
       (`fromB`-`toB`).
       
       When `individual` is true, adjacent changes are reported
       separately.
       */
       iterChanges(f, individual = false) {
           iterChanges(this, f, individual);
       }
       /**
       Get a [change description](https://codemirror.net/6/docs/ref/#state.ChangeDesc) for this change
       set.
       */
       get desc() { return ChangeDesc.create(this.sections); }
       /**
       @internal
       */
       filter(ranges) {
           let resultSections = [], resultInserted = [], filteredSections = [];
           let iter = new SectionIter(this);
           done: for (let i = 0, pos = 0;;) {
               let next = i == ranges.length ? 1e9 : ranges[i++];
               while (pos < next || pos == next && iter.len == 0) {
                   if (iter.done)
                       break done;
                   let len = Math.min(iter.len, next - pos);
                   addSection(filteredSections, len, -1);
                   let ins = iter.ins == -1 ? -1 : iter.off == 0 ? iter.ins : 0;
                   addSection(resultSections, len, ins);
                   if (ins > 0)
                       addInsert(resultInserted, resultSections, iter.text);
                   iter.forward(len);
                   pos += len;
               }
               let end = ranges[i++];
               while (pos < end) {
                   if (iter.done)
                       break done;
                   let len = Math.min(iter.len, end - pos);
                   addSection(resultSections, len, -1);
                   addSection(filteredSections, len, iter.ins == -1 ? -1 : iter.off == 0 ? iter.ins : 0);
                   iter.forward(len);
                   pos += len;
               }
           }
           return { changes: new ChangeSet(resultSections, resultInserted),
               filtered: ChangeDesc.create(filteredSections) };
       }
       /**
       Serialize this change set to a JSON-representable value.
       */
       toJSON() {
           let parts = [];
           for (let i = 0; i < this.sections.length; i += 2) {
               let len = this.sections[i], ins = this.sections[i + 1];
               if (ins < 0)
                   parts.push(len);
               else if (ins == 0)
                   parts.push([len]);
               else
                   parts.push([len].concat(this.inserted[i >> 1].toJSON()));
           }
           return parts;
       }
       /**
       Create a change set for the given changes, for a document of the
       given length, using `lineSep` as line separator.
       */
       static of(changes, length, lineSep) {
           let sections = [], inserted = [], pos = 0;
           let total = null;
           function flush(force = false) {
               if (!force && !sections.length)
                   return;
               if (pos < length)
                   addSection(sections, length - pos, -1);
               let set = new ChangeSet(sections, inserted);
               total = total ? total.compose(set.map(total)) : set;
               sections = [];
               inserted = [];
               pos = 0;
           }
           function process(spec) {
               if (Array.isArray(spec)) {
                   for (let sub of spec)
                       process(sub);
               }
               else if (spec instanceof ChangeSet) {
                   if (spec.length != length)
                       throw new RangeError(`Mismatched change set length (got ${spec.length}, expected ${length})`);
                   flush();
                   total = total ? total.compose(spec.map(total)) : spec;
               }
               else {
                   let { from, to = from, insert } = spec;
                   if (from > to || from < 0 || to > length)
                       throw new RangeError(`Invalid change range ${from} to ${to} (in doc of length ${length})`);
                   let insText = !insert ? Text.empty : typeof insert == "string" ? Text.of(insert.split(lineSep || DefaultSplit)) : insert;
                   let insLen = insText.length;
                   if (from == to && insLen == 0)
                       return;
                   if (from < pos)
                       flush();
                   if (from > pos)
                       addSection(sections, from - pos, -1);
                   addSection(sections, to - from, insLen);
                   addInsert(inserted, sections, insText);
                   pos = to;
               }
           }
           process(changes);
           flush(!total);
           return total;
       }
       /**
       Create an empty changeset of the given length.
       */
       static empty(length) {
           return new ChangeSet(length ? [length, -1] : [], []);
       }
       /**
       Create a changeset from its JSON representation (as produced by
       [`toJSON`](https://codemirror.net/6/docs/ref/#state.ChangeSet.toJSON).
       */
       static fromJSON(json) {
           if (!Array.isArray(json))
               throw new RangeError("Invalid JSON representation of ChangeSet");
           let sections = [], inserted = [];
           for (let i = 0; i < json.length; i++) {
               let part = json[i];
               if (typeof part == "number") {
                   sections.push(part, -1);
               }
               else if (!Array.isArray(part) || typeof part[0] != "number" || part.some((e, i) => i && typeof e != "string")) {
                   throw new RangeError("Invalid JSON representation of ChangeSet");
               }
               else if (part.length == 1) {
                   sections.push(part[0], 0);
               }
               else {
                   while (inserted.length < i)
                       inserted.push(Text.empty);
                   inserted[i] = Text.of(part.slice(1));
                   sections.push(part[0], inserted[i].length);
               }
           }
           return new ChangeSet(sections, inserted);
       }
       /**
       @internal
       */
       static createSet(sections, inserted) {
           return new ChangeSet(sections, inserted);
       }
   }
   function addSection(sections, len, ins, forceJoin = false) {
       if (len == 0 && ins <= 0)
           return;
       let last = sections.length - 2;
       if (last >= 0 && ins <= 0 && ins == sections[last + 1])
           sections[last] += len;
       else if (len == 0 && sections[last] == 0)
           sections[last + 1] += ins;
       else if (forceJoin) {
           sections[last] += len;
           sections[last + 1] += ins;
       }
       else
           sections.push(len, ins);
   }
   function addInsert(values, sections, value) {
       if (value.length == 0)
           return;
       let index = (sections.length - 2) >> 1;
       if (index < values.length) {
           values[values.length - 1] = values[values.length - 1].append(value);
       }
       else {
           while (values.length < index)
               values.push(Text.empty);
           values.push(value);
       }
   }
   function iterChanges(desc, f, individual) {
       let inserted = desc.inserted;
       for (let posA = 0, posB = 0, i = 0; i < desc.sections.length;) {
           let len = desc.sections[i++], ins = desc.sections[i++];
           if (ins < 0) {
               posA += len;
               posB += len;
           }
           else {
               let endA = posA, endB = posB, text = Text.empty;
               for (;;) {
                   endA += len;
                   endB += ins;
                   if (ins && inserted)
                       text = text.append(inserted[(i - 2) >> 1]);
                   if (individual || i == desc.sections.length || desc.sections[i + 1] < 0)
                       break;
                   len = desc.sections[i++];
                   ins = desc.sections[i++];
               }
               f(posA, endA, posB, endB, text);
               posA = endA;
               posB = endB;
           }
       }
   }
   function mapSet(setA, setB, before, mkSet = false) {
       // Produce a copy of setA that applies to the document after setB
       // has been applied (assuming both start at the same document).
       let sections = [], insert = mkSet ? [] : null;
       let a = new SectionIter(setA), b = new SectionIter(setB);
       // Iterate over both sets in parallel. inserted tracks, for changes
       // in A that have to be processed piece-by-piece, whether their
       // content has been inserted already, and refers to the section
       // index.
       for (let inserted = -1;;) {
           if (a.ins == -1 && b.ins == -1) {
               // Move across ranges skipped by both sets.
               let len = Math.min(a.len, b.len);
               addSection(sections, len, -1);
               a.forward(len);
               b.forward(len);
           }
           else if (b.ins >= 0 && (a.ins < 0 || inserted == a.i || a.off == 0 && (b.len < a.len || b.len == a.len && !before))) {
               // If there's a change in B that comes before the next change in
               // A (ordered by start pos, then len, then before flag), skip
               // that (and process any changes in A it covers).
               let len = b.len;
               addSection(sections, b.ins, -1);
               while (len) {
                   let piece = Math.min(a.len, len);
                   if (a.ins >= 0 && inserted < a.i && a.len <= piece) {
                       addSection(sections, 0, a.ins);
                       if (insert)
                           addInsert(insert, sections, a.text);
                       inserted = a.i;
                   }
                   a.forward(piece);
                   len -= piece;
               }
               b.next();
           }
           else if (a.ins >= 0) {
               // Process the part of a change in A up to the start of the next
               // non-deletion change in B (if overlapping).
               let len = 0, left = a.len;
               while (left) {
                   if (b.ins == -1) {
                       let piece = Math.min(left, b.len);
                       len += piece;
                       left -= piece;
                       b.forward(piece);
                   }
                   else if (b.ins == 0 && b.len < left) {
                       left -= b.len;
                       b.next();
                   }
                   else {
                       break;
                   }
               }
               addSection(sections, len, inserted < a.i ? a.ins : 0);
               if (insert && inserted < a.i)
                   addInsert(insert, sections, a.text);
               inserted = a.i;
               a.forward(a.len - left);
           }
           else if (a.done && b.done) {
               return insert ? ChangeSet.createSet(sections, insert) : ChangeDesc.create(sections);
           }
           else {
               throw new Error("Mismatched change set lengths");
           }
       }
   }
   function composeSets(setA, setB, mkSet = false) {
       let sections = [];
       let insert = mkSet ? [] : null;
       let a = new SectionIter(setA), b = new SectionIter(setB);
       for (let open = false;;) {
           if (a.done && b.done) {
               return insert ? ChangeSet.createSet(sections, insert) : ChangeDesc.create(sections);
           }
           else if (a.ins == 0) { // Deletion in A
               addSection(sections, a.len, 0, open);
               a.next();
           }
           else if (b.len == 0 && !b.done) { // Insertion in B
               addSection(sections, 0, b.ins, open);
               if (insert)
                   addInsert(insert, sections, b.text);
               b.next();
           }
           else if (a.done || b.done) {
               throw new Error("Mismatched change set lengths");
           }
           else {
               let len = Math.min(a.len2, b.len), sectionLen = sections.length;
               if (a.ins == -1) {
                   let insB = b.ins == -1 ? -1 : b.off ? 0 : b.ins;
                   addSection(sections, len, insB, open);
                   if (insert && insB)
                       addInsert(insert, sections, b.text);
               }
               else if (b.ins == -1) {
                   addSection(sections, a.off ? 0 : a.len, len, open);
                   if (insert)
                       addInsert(insert, sections, a.textBit(len));
               }
               else {
                   addSection(sections, a.off ? 0 : a.len, b.off ? 0 : b.ins, open);
                   if (insert && !b.off)
                       addInsert(insert, sections, b.text);
               }
               open = (a.ins > len || b.ins >= 0 && b.len > len) && (open || sections.length > sectionLen);
               a.forward2(len);
               b.forward(len);
           }
       }
   }
   class SectionIter {
       constructor(set) {
           this.set = set;
           this.i = 0;
           this.next();
       }
       next() {
           let { sections } = this.set;
           if (this.i < sections.length) {
               this.len = sections[this.i++];
               this.ins = sections[this.i++];
           }
           else {
               this.len = 0;
               this.ins = -2;
           }
           this.off = 0;
       }
       get done() { return this.ins == -2; }
       get len2() { return this.ins < 0 ? this.len : this.ins; }
       get text() {
           let { inserted } = this.set, index = (this.i - 2) >> 1;
           return index >= inserted.length ? Text.empty : inserted[index];
       }
       textBit(len) {
           let { inserted } = this.set, index = (this.i - 2) >> 1;
           return index >= inserted.length && !len ? Text.empty
               : inserted[index].slice(this.off, len == null ? undefined : this.off + len);
       }
       forward(len) {
           if (len == this.len)
               this.next();
           else {
               this.len -= len;
               this.off += len;
           }
       }
       forward2(len) {
           if (this.ins == -1)
               this.forward(len);
           else if (len == this.ins)
               this.next();
           else {
               this.ins -= len;
               this.off += len;
           }
       }
   }

   /**
   A single selection range. When
   [`allowMultipleSelections`](https://codemirror.net/6/docs/ref/#state.EditorState^allowMultipleSelections)
   is enabled, a [selection](https://codemirror.net/6/docs/ref/#state.EditorSelection) may hold
   multiple ranges. By default, selections hold exactly one range.
   */
   class SelectionRange {
       constructor(
       /**
       The lower boundary of the range.
       */
       from, 
       /**
       The upper boundary of the range.
       */
       to, flags) {
           this.from = from;
           this.to = to;
           this.flags = flags;
       }
       /**
       The anchor of the range—the side that doesn't move when you
       extend it.
       */
       get anchor() { return this.flags & 16 /* Inverted */ ? this.to : this.from; }
       /**
       The head of the range, which is moved when the range is
       [extended](https://codemirror.net/6/docs/ref/#state.SelectionRange.extend).
       */
       get head() { return this.flags & 16 /* Inverted */ ? this.from : this.to; }
       /**
       True when `anchor` and `head` are at the same position.
       */
       get empty() { return this.from == this.to; }
       /**
       If this is a cursor that is explicitly associated with the
       character on one of its sides, this returns the side. -1 means
       the character before its position, 1 the character after, and 0
       means no association.
       */
       get assoc() { return this.flags & 4 /* AssocBefore */ ? -1 : this.flags & 8 /* AssocAfter */ ? 1 : 0; }
       /**
       The bidirectional text level associated with this cursor, if
       any.
       */
       get bidiLevel() {
           let level = this.flags & 3 /* BidiLevelMask */;
           return level == 3 ? null : level;
       }
       /**
       The goal column (stored vertical offset) associated with a
       cursor. This is used to preserve the vertical position when
       [moving](https://codemirror.net/6/docs/ref/#view.EditorView.moveVertically) across
       lines of different length.
       */
       get goalColumn() {
           let value = this.flags >> 5 /* GoalColumnOffset */;
           return value == 33554431 /* NoGoalColumn */ ? undefined : value;
       }
       /**
       Map this range through a change, producing a valid range in the
       updated document.
       */
       map(change, assoc = -1) {
           let from, to;
           if (this.empty) {
               from = to = change.mapPos(this.from, assoc);
           }
           else {
               from = change.mapPos(this.from, 1);
               to = change.mapPos(this.to, -1);
           }
           return from == this.from && to == this.to ? this : new SelectionRange(from, to, this.flags);
       }
       /**
       Extend this range to cover at least `from` to `to`.
       */
       extend(from, to = from) {
           if (from <= this.anchor && to >= this.anchor)
               return EditorSelection.range(from, to);
           let head = Math.abs(from - this.anchor) > Math.abs(to - this.anchor) ? from : to;
           return EditorSelection.range(this.anchor, head);
       }
       /**
       Compare this range to another range.
       */
       eq(other) {
           return this.anchor == other.anchor && this.head == other.head;
       }
       /**
       Return a JSON-serializable object representing the range.
       */
       toJSON() { return { anchor: this.anchor, head: this.head }; }
       /**
       Convert a JSON representation of a range to a `SelectionRange`
       instance.
       */
       static fromJSON(json) {
           if (!json || typeof json.anchor != "number" || typeof json.head != "number")
               throw new RangeError("Invalid JSON representation for SelectionRange");
           return EditorSelection.range(json.anchor, json.head);
       }
       /**
       @internal
       */
       static create(from, to, flags) {
           return new SelectionRange(from, to, flags);
       }
   }
   /**
   An editor selection holds one or more selection ranges.
   */
   class EditorSelection {
       constructor(
       /**
       The ranges in the selection, sorted by position. Ranges cannot
       overlap (but they may touch, if they aren't empty).
       */
       ranges, 
       /**
       The index of the _main_ range in the selection (which is
       usually the range that was added last).
       */
       mainIndex) {
           this.ranges = ranges;
           this.mainIndex = mainIndex;
       }
       /**
       Map a selection through a change. Used to adjust the selection
       position for changes.
       */
       map(change, assoc = -1) {
           if (change.empty)
               return this;
           return EditorSelection.create(this.ranges.map(r => r.map(change, assoc)), this.mainIndex);
       }
       /**
       Compare this selection to another selection.
       */
       eq(other) {
           if (this.ranges.length != other.ranges.length ||
               this.mainIndex != other.mainIndex)
               return false;
           for (let i = 0; i < this.ranges.length; i++)
               if (!this.ranges[i].eq(other.ranges[i]))
                   return false;
           return true;
       }
       /**
       Get the primary selection range. Usually, you should make sure
       your code applies to _all_ ranges, by using methods like
       [`changeByRange`](https://codemirror.net/6/docs/ref/#state.EditorState.changeByRange).
       */
       get main() { return this.ranges[this.mainIndex]; }
       /**
       Make sure the selection only has one range. Returns a selection
       holding only the main range from this selection.
       */
       asSingle() {
           return this.ranges.length == 1 ? this : new EditorSelection([this.main], 0);
       }
       /**
       Extend this selection with an extra range.
       */
       addRange(range, main = true) {
           return EditorSelection.create([range].concat(this.ranges), main ? 0 : this.mainIndex + 1);
       }
       /**
       Replace a given range with another range, and then normalize the
       selection to merge and sort ranges if necessary.
       */
       replaceRange(range, which = this.mainIndex) {
           let ranges = this.ranges.slice();
           ranges[which] = range;
           return EditorSelection.create(ranges, this.mainIndex);
       }
       /**
       Convert this selection to an object that can be serialized to
       JSON.
       */
       toJSON() {
           return { ranges: this.ranges.map(r => r.toJSON()), main: this.mainIndex };
       }
       /**
       Create a selection from a JSON representation.
       */
       static fromJSON(json) {
           if (!json || !Array.isArray(json.ranges) || typeof json.main != "number" || json.main >= json.ranges.length)
               throw new RangeError("Invalid JSON representation for EditorSelection");
           return new EditorSelection(json.ranges.map((r) => SelectionRange.fromJSON(r)), json.main);
       }
       /**
       Create a selection holding a single range.
       */
       static single(anchor, head = anchor) {
           return new EditorSelection([EditorSelection.range(anchor, head)], 0);
       }
       /**
       Sort and merge the given set of ranges, creating a valid
       selection.
       */
       static create(ranges, mainIndex = 0) {
           if (ranges.length == 0)
               throw new RangeError("A selection needs at least one range");
           for (let pos = 0, i = 0; i < ranges.length; i++) {
               let range = ranges[i];
               if (range.empty ? range.from <= pos : range.from < pos)
                   return EditorSelection.normalized(ranges.slice(), mainIndex);
               pos = range.to;
           }
           return new EditorSelection(ranges, mainIndex);
       }
       /**
       Create a cursor selection range at the given position. You can
       safely ignore the optional arguments in most situations.
       */
       static cursor(pos, assoc = 0, bidiLevel, goalColumn) {
           return SelectionRange.create(pos, pos, (assoc == 0 ? 0 : assoc < 0 ? 4 /* AssocBefore */ : 8 /* AssocAfter */) |
               (bidiLevel == null ? 3 : Math.min(2, bidiLevel)) |
               ((goalColumn !== null && goalColumn !== void 0 ? goalColumn : 33554431 /* NoGoalColumn */) << 5 /* GoalColumnOffset */));
       }
       /**
       Create a selection range.
       */
       static range(anchor, head, goalColumn) {
           let goal = (goalColumn !== null && goalColumn !== void 0 ? goalColumn : 33554431 /* NoGoalColumn */) << 5 /* GoalColumnOffset */;
           return head < anchor ? SelectionRange.create(head, anchor, 16 /* Inverted */ | goal | 8 /* AssocAfter */)
               : SelectionRange.create(anchor, head, goal | (head > anchor ? 4 /* AssocBefore */ : 0));
       }
       /**
       @internal
       */
       static normalized(ranges, mainIndex = 0) {
           let main = ranges[mainIndex];
           ranges.sort((a, b) => a.from - b.from);
           mainIndex = ranges.indexOf(main);
           for (let i = 1; i < ranges.length; i++) {
               let range = ranges[i], prev = ranges[i - 1];
               if (range.empty ? range.from <= prev.to : range.from < prev.to) {
                   let from = prev.from, to = Math.max(range.to, prev.to);
                   if (i <= mainIndex)
                       mainIndex--;
                   ranges.splice(--i, 2, range.anchor > range.head ? EditorSelection.range(to, from) : EditorSelection.range(from, to));
               }
           }
           return new EditorSelection(ranges, mainIndex);
       }
   }
   function checkSelection(selection, docLength) {
       for (let range of selection.ranges)
           if (range.to > docLength)
               throw new RangeError("Selection points outside of document");
   }

   let nextID = 0;
   /**
   A facet is a labeled value that is associated with an editor
   state. It takes inputs from any number of extensions, and combines
   those into a single output value.

   Examples of uses of facets are the [tab
   size](https://codemirror.net/6/docs/ref/#state.EditorState^tabSize), [editor
   attributes](https://codemirror.net/6/docs/ref/#view.EditorView^editorAttributes), and [update
   listeners](https://codemirror.net/6/docs/ref/#view.EditorView^updateListener).
   */
   class Facet {
       constructor(
       /**
       @internal
       */
       combine, 
       /**
       @internal
       */
       compareInput, 
       /**
       @internal
       */
       compare, isStatic, enables) {
           this.combine = combine;
           this.compareInput = compareInput;
           this.compare = compare;
           this.isStatic = isStatic;
           /**
           @internal
           */
           this.id = nextID++;
           this.default = combine([]);
           this.extensions = typeof enables == "function" ? enables(this) : enables;
       }
       /**
       Define a new facet.
       */
       static define(config = {}) {
           return new Facet(config.combine || ((a) => a), config.compareInput || ((a, b) => a === b), config.compare || (!config.combine ? sameArray$1 : (a, b) => a === b), !!config.static, config.enables);
       }
       /**
       Returns an extension that adds the given value to this facet.
       */
       of(value) {
           return new FacetProvider([], this, 0 /* Static */, value);
       }
       /**
       Create an extension that computes a value for the facet from a
       state. You must take care to declare the parts of the state that
       this value depends on, since your function is only called again
       for a new state when one of those parts changed.
       
       In cases where your value depends only on a single field, you'll
       want to use the [`from`](https://codemirror.net/6/docs/ref/#state.Facet.from) method instead.
       */
       compute(deps, get) {
           if (this.isStatic)
               throw new Error("Can't compute a static facet");
           return new FacetProvider(deps, this, 1 /* Single */, get);
       }
       /**
       Create an extension that computes zero or more values for this
       facet from a state.
       */
       computeN(deps, get) {
           if (this.isStatic)
               throw new Error("Can't compute a static facet");
           return new FacetProvider(deps, this, 2 /* Multi */, get);
       }
       from(field, get) {
           if (!get)
               get = x => x;
           return this.compute([field], state => get(state.field(field)));
       }
   }
   function sameArray$1(a, b) {
       return a == b || a.length == b.length && a.every((e, i) => e === b[i]);
   }
   class FacetProvider {
       constructor(dependencies, facet, type, value) {
           this.dependencies = dependencies;
           this.facet = facet;
           this.type = type;
           this.value = value;
           this.id = nextID++;
       }
       dynamicSlot(addresses) {
           var _a;
           let getter = this.value;
           let compare = this.facet.compareInput;
           let id = this.id, idx = addresses[id] >> 1, multi = this.type == 2 /* Multi */;
           let depDoc = false, depSel = false, depAddrs = [];
           for (let dep of this.dependencies) {
               if (dep == "doc")
                   depDoc = true;
               else if (dep == "selection")
                   depSel = true;
               else if ((((_a = addresses[dep.id]) !== null && _a !== void 0 ? _a : 1) & 1) == 0)
                   depAddrs.push(addresses[dep.id]);
           }
           return {
               create(state) {
                   state.values[idx] = getter(state);
                   return 1 /* Changed */;
               },
               update(state, tr) {
                   if ((depDoc && tr.docChanged) || (depSel && (tr.docChanged || tr.selection)) || ensureAll(state, depAddrs)) {
                       let newVal = getter(state);
                       if (multi ? !compareArray(newVal, state.values[idx], compare) : !compare(newVal, state.values[idx])) {
                           state.values[idx] = newVal;
                           return 1 /* Changed */;
                       }
                   }
                   return 0;
               },
               reconfigure: (state, oldState) => {
                   let newVal = getter(state);
                   let oldAddr = oldState.config.address[id];
                   if (oldAddr != null) {
                       let oldVal = getAddr(oldState, oldAddr);
                       if (this.dependencies.every(dep => {
                           return dep instanceof Facet ? oldState.facet(dep) === state.facet(dep) :
                               dep instanceof StateField ? oldState.field(dep, false) == state.field(dep, false) : true;
                       }) || (multi ? compareArray(newVal, oldVal, compare) : compare(newVal, oldVal))) {
                           state.values[idx] = oldVal;
                           return 0;
                       }
                   }
                   state.values[idx] = newVal;
                   return 1 /* Changed */;
               }
           };
       }
   }
   function compareArray(a, b, compare) {
       if (a.length != b.length)
           return false;
       for (let i = 0; i < a.length; i++)
           if (!compare(a[i], b[i]))
               return false;
       return true;
   }
   function ensureAll(state, addrs) {
       let changed = false;
       for (let addr of addrs)
           if (ensureAddr(state, addr) & 1 /* Changed */)
               changed = true;
       return changed;
   }
   function dynamicFacetSlot(addresses, facet, providers) {
       let providerAddrs = providers.map(p => addresses[p.id]);
       let providerTypes = providers.map(p => p.type);
       let dynamic = providerAddrs.filter(p => !(p & 1));
       let idx = addresses[facet.id] >> 1;
       function get(state) {
           let values = [];
           for (let i = 0; i < providerAddrs.length; i++) {
               let value = getAddr(state, providerAddrs[i]);
               if (providerTypes[i] == 2 /* Multi */)
                   for (let val of value)
                       values.push(val);
               else
                   values.push(value);
           }
           return facet.combine(values);
       }
       return {
           create(state) {
               for (let addr of providerAddrs)
                   ensureAddr(state, addr);
               state.values[idx] = get(state);
               return 1 /* Changed */;
           },
           update(state, tr) {
               if (!ensureAll(state, dynamic))
                   return 0;
               let value = get(state);
               if (facet.compare(value, state.values[idx]))
                   return 0;
               state.values[idx] = value;
               return 1 /* Changed */;
           },
           reconfigure(state, oldState) {
               let depChanged = ensureAll(state, providerAddrs);
               let oldProviders = oldState.config.facets[facet.id], oldValue = oldState.facet(facet);
               if (oldProviders && !depChanged && sameArray$1(providers, oldProviders)) {
                   state.values[idx] = oldValue;
                   return 0;
               }
               let value = get(state);
               if (facet.compare(value, oldValue)) {
                   state.values[idx] = oldValue;
                   return 0;
               }
               state.values[idx] = value;
               return 1 /* Changed */;
           }
       };
   }
   const initField = /*@__PURE__*/Facet.define({ static: true });
   /**
   Fields can store additional information in an editor state, and
   keep it in sync with the rest of the state.
   */
   class StateField {
       constructor(
       /**
       @internal
       */
       id, createF, updateF, compareF, 
       /**
       @internal
       */
       spec) {
           this.id = id;
           this.createF = createF;
           this.updateF = updateF;
           this.compareF = compareF;
           this.spec = spec;
           /**
           @internal
           */
           this.provides = undefined;
       }
       /**
       Define a state field.
       */
       static define(config) {
           let field = new StateField(nextID++, config.create, config.update, config.compare || ((a, b) => a === b), config);
           if (config.provide)
               field.provides = config.provide(field);
           return field;
       }
       create(state) {
           let init = state.facet(initField).find(i => i.field == this);
           return ((init === null || init === void 0 ? void 0 : init.create) || this.createF)(state);
       }
       /**
       @internal
       */
       slot(addresses) {
           let idx = addresses[this.id] >> 1;
           return {
               create: (state) => {
                   state.values[idx] = this.create(state);
                   return 1 /* Changed */;
               },
               update: (state, tr) => {
                   let oldVal = state.values[idx];
                   let value = this.updateF(oldVal, tr);
                   if (this.compareF(oldVal, value))
                       return 0;
                   state.values[idx] = value;
                   return 1 /* Changed */;
               },
               reconfigure: (state, oldState) => {
                   if (oldState.config.address[this.id] != null) {
                       state.values[idx] = oldState.field(this);
                       return 0;
                   }
                   state.values[idx] = this.create(state);
                   return 1 /* Changed */;
               }
           };
       }
       /**
       Returns an extension that enables this field and overrides the
       way it is initialized. Can be useful when you need to provide a
       non-default starting value for the field.
       */
       init(create) {
           return [this, initField.of({ field: this, create })];
       }
       /**
       State field instances can be used as
       [`Extension`](https://codemirror.net/6/docs/ref/#state.Extension) values to enable the field in a
       given state.
       */
       get extension() { return this; }
   }
   const Prec_ = { lowest: 4, low: 3, default: 2, high: 1, highest: 0 };
   function prec(value) {
       return (ext) => new PrecExtension(ext, value);
   }
   /**
   By default extensions are registered in the order they are found
   in the flattened form of nested array that was provided.
   Individual extension values can be assigned a precedence to
   override this. Extensions that do not have a precedence set get
   the precedence of the nearest parent with a precedence, or
   [`default`](https://codemirror.net/6/docs/ref/#state.Prec.default) if there is no such parent. The
   final ordering of extensions is determined by first sorting by
   precedence and then by order within each precedence.
   */
   const Prec = {
       /**
       The highest precedence level, for extensions that should end up
       near the start of the precedence ordering.
       */
       highest: /*@__PURE__*/prec(Prec_.highest),
       /**
       A higher-than-default precedence, for extensions that should
       come before those with default precedence.
       */
       high: /*@__PURE__*/prec(Prec_.high),
       /**
       The default precedence, which is also used for extensions
       without an explicit precedence.
       */
       default: /*@__PURE__*/prec(Prec_.default),
       /**
       A lower-than-default precedence.
       */
       low: /*@__PURE__*/prec(Prec_.low),
       /**
       The lowest precedence level. Meant for things that should end up
       near the end of the extension order.
       */
       lowest: /*@__PURE__*/prec(Prec_.lowest)
   };
   class PrecExtension {
       constructor(inner, prec) {
           this.inner = inner;
           this.prec = prec;
       }
   }
   /**
   Extension compartments can be used to make a configuration
   dynamic. By [wrapping](https://codemirror.net/6/docs/ref/#state.Compartment.of) part of your
   configuration in a compartment, you can later
   [replace](https://codemirror.net/6/docs/ref/#state.Compartment.reconfigure) that part through a
   transaction.
   */
   class Compartment {
       /**
       Create an instance of this compartment to add to your [state
       configuration](https://codemirror.net/6/docs/ref/#state.EditorStateConfig.extensions).
       */
       of(ext) { return new CompartmentInstance(this, ext); }
       /**
       Create an [effect](https://codemirror.net/6/docs/ref/#state.TransactionSpec.effects) that
       reconfigures this compartment.
       */
       reconfigure(content) {
           return Compartment.reconfigure.of({ compartment: this, extension: content });
       }
       /**
       Get the current content of the compartment in the state, or
       `undefined` if it isn't present.
       */
       get(state) {
           return state.config.compartments.get(this);
       }
   }
   class CompartmentInstance {
       constructor(compartment, inner) {
           this.compartment = compartment;
           this.inner = inner;
       }
   }
   class Configuration {
       constructor(base, compartments, dynamicSlots, address, staticValues, facets) {
           this.base = base;
           this.compartments = compartments;
           this.dynamicSlots = dynamicSlots;
           this.address = address;
           this.staticValues = staticValues;
           this.facets = facets;
           this.statusTemplate = [];
           while (this.statusTemplate.length < dynamicSlots.length)
               this.statusTemplate.push(0 /* Unresolved */);
       }
       staticFacet(facet) {
           let addr = this.address[facet.id];
           return addr == null ? facet.default : this.staticValues[addr >> 1];
       }
       static resolve(base, compartments, oldState) {
           let fields = [];
           let facets = Object.create(null);
           let newCompartments = new Map();
           for (let ext of flatten(base, compartments, newCompartments)) {
               if (ext instanceof StateField)
                   fields.push(ext);
               else
                   (facets[ext.facet.id] || (facets[ext.facet.id] = [])).push(ext);
           }
           let address = Object.create(null);
           let staticValues = [];
           let dynamicSlots = [];
           for (let field of fields) {
               address[field.id] = dynamicSlots.length << 1;
               dynamicSlots.push(a => field.slot(a));
           }
           let oldFacets = oldState === null || oldState === void 0 ? void 0 : oldState.config.facets;
           for (let id in facets) {
               let providers = facets[id], facet = providers[0].facet;
               let oldProviders = oldFacets && oldFacets[id] || [];
               if (providers.every(p => p.type == 0 /* Static */)) {
                   address[facet.id] = (staticValues.length << 1) | 1;
                   if (sameArray$1(oldProviders, providers)) {
                       staticValues.push(oldState.facet(facet));
                   }
                   else {
                       let value = facet.combine(providers.map(p => p.value));
                       staticValues.push(oldState && facet.compare(value, oldState.facet(facet)) ? oldState.facet(facet) : value);
                   }
               }
               else {
                   for (let p of providers) {
                       if (p.type == 0 /* Static */) {
                           address[p.id] = (staticValues.length << 1) | 1;
                           staticValues.push(p.value);
                       }
                       else {
                           address[p.id] = dynamicSlots.length << 1;
                           dynamicSlots.push(a => p.dynamicSlot(a));
                       }
                   }
                   address[facet.id] = dynamicSlots.length << 1;
                   dynamicSlots.push(a => dynamicFacetSlot(a, facet, providers));
               }
           }
           let dynamic = dynamicSlots.map(f => f(address));
           return new Configuration(base, newCompartments, dynamic, address, staticValues, facets);
       }
   }
   function flatten(extension, compartments, newCompartments) {
       let result = [[], [], [], [], []];
       let seen = new Map();
       function inner(ext, prec) {
           let known = seen.get(ext);
           if (known != null) {
               if (known <= prec)
                   return;
               let found = result[known].indexOf(ext);
               if (found > -1)
                   result[known].splice(found, 1);
               if (ext instanceof CompartmentInstance)
                   newCompartments.delete(ext.compartment);
           }
           seen.set(ext, prec);
           if (Array.isArray(ext)) {
               for (let e of ext)
                   inner(e, prec);
           }
           else if (ext instanceof CompartmentInstance) {
               if (newCompartments.has(ext.compartment))
                   throw new RangeError(`Duplicate use of compartment in extensions`);
               let content = compartments.get(ext.compartment) || ext.inner;
               newCompartments.set(ext.compartment, content);
               inner(content, prec);
           }
           else if (ext instanceof PrecExtension) {
               inner(ext.inner, ext.prec);
           }
           else if (ext instanceof StateField) {
               result[prec].push(ext);
               if (ext.provides)
                   inner(ext.provides, prec);
           }
           else if (ext instanceof FacetProvider) {
               result[prec].push(ext);
               if (ext.facet.extensions)
                   inner(ext.facet.extensions, Prec_.default);
           }
           else {
               let content = ext.extension;
               if (!content)
                   throw new Error(`Unrecognized extension value in extension set (${ext}). This sometimes happens because multiple instances of @codemirror/state are loaded, breaking instanceof checks.`);
               inner(content, prec);
           }
       }
       inner(extension, Prec_.default);
       return result.reduce((a, b) => a.concat(b));
   }
   function ensureAddr(state, addr) {
       if (addr & 1)
           return 2 /* Computed */;
       let idx = addr >> 1;
       let status = state.status[idx];
       if (status == 4 /* Computing */)
           throw new Error("Cyclic dependency between fields and/or facets");
       if (status & 2 /* Computed */)
           return status;
       state.status[idx] = 4 /* Computing */;
       let changed = state.computeSlot(state, state.config.dynamicSlots[idx]);
       return state.status[idx] = 2 /* Computed */ | changed;
   }
   function getAddr(state, addr) {
       return addr & 1 ? state.config.staticValues[addr >> 1] : state.values[addr >> 1];
   }

   const languageData = /*@__PURE__*/Facet.define();
   const allowMultipleSelections = /*@__PURE__*/Facet.define({
       combine: values => values.some(v => v),
       static: true
   });
   const lineSeparator = /*@__PURE__*/Facet.define({
       combine: values => values.length ? values[0] : undefined,
       static: true
   });
   const changeFilter = /*@__PURE__*/Facet.define();
   const transactionFilter = /*@__PURE__*/Facet.define();
   const transactionExtender = /*@__PURE__*/Facet.define();
   const readOnly = /*@__PURE__*/Facet.define({
       combine: values => values.length ? values[0] : false
   });

   /**
   Annotations are tagged values that are used to add metadata to
   transactions in an extensible way. They should be used to model
   things that effect the entire transaction (such as its [time
   stamp](https://codemirror.net/6/docs/ref/#state.Transaction^time) or information about its
   [origin](https://codemirror.net/6/docs/ref/#state.Transaction^userEvent)). For effects that happen
   _alongside_ the other changes made by the transaction, [state
   effects](https://codemirror.net/6/docs/ref/#state.StateEffect) are more appropriate.
   */
   class Annotation {
       /**
       @internal
       */
       constructor(
       /**
       The annotation type.
       */
       type, 
       /**
       The value of this annotation.
       */
       value) {
           this.type = type;
           this.value = value;
       }
       /**
       Define a new type of annotation.
       */
       static define() { return new AnnotationType(); }
   }
   /**
   Marker that identifies a type of [annotation](https://codemirror.net/6/docs/ref/#state.Annotation).
   */
   class AnnotationType {
       /**
       Create an instance of this annotation.
       */
       of(value) { return new Annotation(this, value); }
   }
   /**
   Representation of a type of state effect. Defined with
   [`StateEffect.define`](https://codemirror.net/6/docs/ref/#state.StateEffect^define).
   */
   class StateEffectType {
       /**
       @internal
       */
       constructor(
       // The `any` types in these function types are there to work
       // around TypeScript issue #37631, where the type guard on
       // `StateEffect.is` mysteriously stops working when these properly
       // have type `Value`.
       /**
       @internal
       */
       map) {
           this.map = map;
       }
       /**
       Create a [state effect](https://codemirror.net/6/docs/ref/#state.StateEffect) instance of this
       type.
       */
       of(value) { return new StateEffect(this, value); }
   }
   /**
   State effects can be used to represent additional effects
   associated with a [transaction](https://codemirror.net/6/docs/ref/#state.Transaction.effects). They
   are often useful to model changes to custom [state
   fields](https://codemirror.net/6/docs/ref/#state.StateField), when those changes aren't implicit in
   document or selection changes.
   */
   class StateEffect {
       /**
       @internal
       */
       constructor(
       /**
       @internal
       */
       type, 
       /**
       The value of this effect.
       */
       value) {
           this.type = type;
           this.value = value;
       }
       /**
       Map this effect through a position mapping. Will return
       `undefined` when that ends up deleting the effect.
       */
       map(mapping) {
           let mapped = this.type.map(this.value, mapping);
           return mapped === undefined ? undefined : mapped == this.value ? this : new StateEffect(this.type, mapped);
       }
       /**
       Tells you whether this effect object is of a given
       [type](https://codemirror.net/6/docs/ref/#state.StateEffectType).
       */
       is(type) { return this.type == type; }
       /**
       Define a new effect type. The type parameter indicates the type
       of values that his effect holds.
       */
       static define(spec = {}) {
           return new StateEffectType(spec.map || (v => v));
       }
       /**
       Map an array of effects through a change set.
       */
       static mapEffects(effects, mapping) {
           if (!effects.length)
               return effects;
           let result = [];
           for (let effect of effects) {
               let mapped = effect.map(mapping);
               if (mapped)
                   result.push(mapped);
           }
           return result;
       }
   }
   /**
   This effect can be used to reconfigure the root extensions of
   the editor. Doing this will discard any extensions
   [appended](https://codemirror.net/6/docs/ref/#state.StateEffect^appendConfig), but does not reset
   the content of [reconfigured](https://codemirror.net/6/docs/ref/#state.Compartment.reconfigure)
   compartments.
   */
   StateEffect.reconfigure = /*@__PURE__*/StateEffect.define();
   /**
   Append extensions to the top-level configuration of the editor.
   */
   StateEffect.appendConfig = /*@__PURE__*/StateEffect.define();
   /**
   Changes to the editor state are grouped into transactions.
   Typically, a user action creates a single transaction, which may
   contain any number of document changes, may change the selection,
   or have other effects. Create a transaction by calling
   [`EditorState.update`](https://codemirror.net/6/docs/ref/#state.EditorState.update), or immediately
   dispatch one by calling
   [`EditorView.dispatch`](https://codemirror.net/6/docs/ref/#view.EditorView.dispatch).
   */
   class Transaction {
       constructor(
       /**
       The state from which the transaction starts.
       */
       startState, 
       /**
       The document changes made by this transaction.
       */
       changes, 
       /**
       The selection set by this transaction, or undefined if it
       doesn't explicitly set a selection.
       */
       selection, 
       /**
       The effects added to the transaction.
       */
       effects, 
       /**
       @internal
       */
       annotations, 
       /**
       Whether the selection should be scrolled into view after this
       transaction is dispatched.
       */
       scrollIntoView) {
           this.startState = startState;
           this.changes = changes;
           this.selection = selection;
           this.effects = effects;
           this.annotations = annotations;
           this.scrollIntoView = scrollIntoView;
           /**
           @internal
           */
           this._doc = null;
           /**
           @internal
           */
           this._state = null;
           if (selection)
               checkSelection(selection, changes.newLength);
           if (!annotations.some((a) => a.type == Transaction.time))
               this.annotations = annotations.concat(Transaction.time.of(Date.now()));
       }
       /**
       @internal
       */
       static create(startState, changes, selection, effects, annotations, scrollIntoView) {
           return new Transaction(startState, changes, selection, effects, annotations, scrollIntoView);
       }
       /**
       The new document produced by the transaction. Contrary to
       [`.state`](https://codemirror.net/6/docs/ref/#state.Transaction.state)`.doc`, accessing this won't
       force the entire new state to be computed right away, so it is
       recommended that [transaction
       filters](https://codemirror.net/6/docs/ref/#state.EditorState^transactionFilter) use this getter
       when they need to look at the new document.
       */
       get newDoc() {
           return this._doc || (this._doc = this.changes.apply(this.startState.doc));
       }
       /**
       The new selection produced by the transaction. If
       [`this.selection`](https://codemirror.net/6/docs/ref/#state.Transaction.selection) is undefined,
       this will [map](https://codemirror.net/6/docs/ref/#state.EditorSelection.map) the start state's
       current selection through the changes made by the transaction.
       */
       get newSelection() {
           return this.selection || this.startState.selection.map(this.changes);
       }
       /**
       The new state created by the transaction. Computed on demand
       (but retained for subsequent access), so it is recommended not to
       access it in [transaction
       filters](https://codemirror.net/6/docs/ref/#state.EditorState^transactionFilter) when possible.
       */
       get state() {
           if (!this._state)
               this.startState.applyTransaction(this);
           return this._state;
       }
       /**
       Get the value of the given annotation type, if any.
       */
       annotation(type) {
           for (let ann of this.annotations)
               if (ann.type == type)
                   return ann.value;
           return undefined;
       }
       /**
       Indicates whether the transaction changed the document.
       */
       get docChanged() { return !this.changes.empty; }
       /**
       Indicates whether this transaction reconfigures the state
       (through a [configuration compartment](https://codemirror.net/6/docs/ref/#state.Compartment) or
       with a top-level configuration
       [effect](https://codemirror.net/6/docs/ref/#state.StateEffect^reconfigure).
       */
       get reconfigured() { return this.startState.config != this.state.config; }
       /**
       Returns true if the transaction has a [user
       event](https://codemirror.net/6/docs/ref/#state.Transaction^userEvent) annotation that is equal to
       or more specific than `event`. For example, if the transaction
       has `"select.pointer"` as user event, `"select"` and
       `"select.pointer"` will match it.
       */
       isUserEvent(event) {
           let e = this.annotation(Transaction.userEvent);
           return !!(e && (e == event || e.length > event.length && e.slice(0, event.length) == event && e[event.length] == "."));
       }
   }
   /**
   Annotation used to store transaction timestamps. Automatically
   added to every transaction, holding `Date.now()`.
   */
   Transaction.time = /*@__PURE__*/Annotation.define();
   /**
   Annotation used to associate a transaction with a user interface
   event. Holds a string identifying the event, using a
   dot-separated format to support attaching more specific
   information. The events used by the core libraries are:

    - `"input"` when content is entered
      - `"input.type"` for typed input
        - `"input.type.compose"` for composition
      - `"input.paste"` for pasted input
      - `"input.drop"` when adding content with drag-and-drop
      - `"input.complete"` when autocompleting
    - `"delete"` when the user deletes content
      - `"delete.selection"` when deleting the selection
      - `"delete.forward"` when deleting forward from the selection
      - `"delete.backward"` when deleting backward from the selection
      - `"delete.cut"` when cutting to the clipboard
    - `"move"` when content is moved
      - `"move.drop"` when content is moved within the editor through drag-and-drop
    - `"select"` when explicitly changing the selection
      - `"select.pointer"` when selecting with a mouse or other pointing device
    - `"undo"` and `"redo"` for history actions

   Use [`isUserEvent`](https://codemirror.net/6/docs/ref/#state.Transaction.isUserEvent) to check
   whether the annotation matches a given event.
   */
   Transaction.userEvent = /*@__PURE__*/Annotation.define();
   /**
   Annotation indicating whether a transaction should be added to
   the undo history or not.
   */
   Transaction.addToHistory = /*@__PURE__*/Annotation.define();
   /**
   Annotation indicating (when present and true) that a transaction
   represents a change made by some other actor, not the user. This
   is used, for example, to tag other people's changes in
   collaborative editing.
   */
   Transaction.remote = /*@__PURE__*/Annotation.define();
   function joinRanges(a, b) {
       let result = [];
       for (let iA = 0, iB = 0;;) {
           let from, to;
           if (iA < a.length && (iB == b.length || b[iB] >= a[iA])) {
               from = a[iA++];
               to = a[iA++];
           }
           else if (iB < b.length) {
               from = b[iB++];
               to = b[iB++];
           }
           else
               return result;
           if (!result.length || result[result.length - 1] < from)
               result.push(from, to);
           else if (result[result.length - 1] < to)
               result[result.length - 1] = to;
       }
   }
   function mergeTransaction(a, b, sequential) {
       var _a;
       let mapForA, mapForB, changes;
       if (sequential) {
           mapForA = b.changes;
           mapForB = ChangeSet.empty(b.changes.length);
           changes = a.changes.compose(b.changes);
       }
       else {
           mapForA = b.changes.map(a.changes);
           mapForB = a.changes.mapDesc(b.changes, true);
           changes = a.changes.compose(mapForA);
       }
       return {
           changes,
           selection: b.selection ? b.selection.map(mapForB) : (_a = a.selection) === null || _a === void 0 ? void 0 : _a.map(mapForA),
           effects: StateEffect.mapEffects(a.effects, mapForA).concat(StateEffect.mapEffects(b.effects, mapForB)),
           annotations: a.annotations.length ? a.annotations.concat(b.annotations) : b.annotations,
           scrollIntoView: a.scrollIntoView || b.scrollIntoView
       };
   }
   function resolveTransactionInner(state, spec, docSize) {
       let sel = spec.selection, annotations = asArray(spec.annotations);
       if (spec.userEvent)
           annotations = annotations.concat(Transaction.userEvent.of(spec.userEvent));
       return {
           changes: spec.changes instanceof ChangeSet ? spec.changes
               : ChangeSet.of(spec.changes || [], docSize, state.facet(lineSeparator)),
           selection: sel && (sel instanceof EditorSelection ? sel : EditorSelection.single(sel.anchor, sel.head)),
           effects: asArray(spec.effects),
           annotations,
           scrollIntoView: !!spec.scrollIntoView
       };
   }
   function resolveTransaction(state, specs, filter) {
       let s = resolveTransactionInner(state, specs.length ? specs[0] : {}, state.doc.length);
       if (specs.length && specs[0].filter === false)
           filter = false;
       for (let i = 1; i < specs.length; i++) {
           if (specs[i].filter === false)
               filter = false;
           let seq = !!specs[i].sequential;
           s = mergeTransaction(s, resolveTransactionInner(state, specs[i], seq ? s.changes.newLength : state.doc.length), seq);
       }
       let tr = Transaction.create(state, s.changes, s.selection, s.effects, s.annotations, s.scrollIntoView);
       return extendTransaction(filter ? filterTransaction(tr) : tr);
   }
   // Finish a transaction by applying filters if necessary.
   function filterTransaction(tr) {
       let state = tr.startState;
       // Change filters
       let result = true;
       for (let filter of state.facet(changeFilter)) {
           let value = filter(tr);
           if (value === false) {
               result = false;
               break;
           }
           if (Array.isArray(value))
               result = result === true ? value : joinRanges(result, value);
       }
       if (result !== true) {
           let changes, back;
           if (result === false) {
               back = tr.changes.invertedDesc;
               changes = ChangeSet.empty(state.doc.length);
           }
           else {
               let filtered = tr.changes.filter(result);
               changes = filtered.changes;
               back = filtered.filtered.mapDesc(filtered.changes).invertedDesc;
           }
           tr = Transaction.create(state, changes, tr.selection && tr.selection.map(back), StateEffect.mapEffects(tr.effects, back), tr.annotations, tr.scrollIntoView);
       }
       // Transaction filters
       let filters = state.facet(transactionFilter);
       for (let i = filters.length - 1; i >= 0; i--) {
           let filtered = filters[i](tr);
           if (filtered instanceof Transaction)
               tr = filtered;
           else if (Array.isArray(filtered) && filtered.length == 1 && filtered[0] instanceof Transaction)
               tr = filtered[0];
           else
               tr = resolveTransaction(state, asArray(filtered), false);
       }
       return tr;
   }
   function extendTransaction(tr) {
       let state = tr.startState, extenders = state.facet(transactionExtender), spec = tr;
       for (let i = extenders.length - 1; i >= 0; i--) {
           let extension = extenders[i](tr);
           if (extension && Object.keys(extension).length)
               spec = mergeTransaction(tr, resolveTransactionInner(state, extension, tr.changes.newLength), true);
       }
       return spec == tr ? tr : Transaction.create(state, tr.changes, tr.selection, spec.effects, spec.annotations, spec.scrollIntoView);
   }
   const none$1 = [];
   function asArray(value) {
       return value == null ? none$1 : Array.isArray(value) ? value : [value];
   }

   /**
   The categories produced by a [character
   categorizer](https://codemirror.net/6/docs/ref/#state.EditorState.charCategorizer). These are used
   do things like selecting by word.
   */
   var CharCategory = /*@__PURE__*/(function (CharCategory) {
       /**
       Word characters.
       */
       CharCategory[CharCategory["Word"] = 0] = "Word";
       /**
       Whitespace.
       */
       CharCategory[CharCategory["Space"] = 1] = "Space";
       /**
       Anything else.
       */
       CharCategory[CharCategory["Other"] = 2] = "Other";
   return CharCategory})(CharCategory || (CharCategory = {}));
   const nonASCIISingleCaseWordChar = /[\u00df\u0587\u0590-\u05f4\u0600-\u06ff\u3040-\u309f\u30a0-\u30ff\u3400-\u4db5\u4e00-\u9fcc\uac00-\ud7af]/;
   let wordChar;
   try {
       wordChar = /*@__PURE__*/new RegExp("[\\p{Alphabetic}\\p{Number}_]", "u");
   }
   catch (_) { }
   function hasWordChar(str) {
       if (wordChar)
           return wordChar.test(str);
       for (let i = 0; i < str.length; i++) {
           let ch = str[i];
           if (/\w/.test(ch) || ch > "\x80" && (ch.toUpperCase() != ch.toLowerCase() || nonASCIISingleCaseWordChar.test(ch)))
               return true;
       }
       return false;
   }
   function makeCategorizer(wordChars) {
       return (char) => {
           if (!/\S/.test(char))
               return CharCategory.Space;
           if (hasWordChar(char))
               return CharCategory.Word;
           for (let i = 0; i < wordChars.length; i++)
               if (char.indexOf(wordChars[i]) > -1)
                   return CharCategory.Word;
           return CharCategory.Other;
       };
   }

   /**
   The editor state class is a persistent (immutable) data structure.
   To update a state, you [create](https://codemirror.net/6/docs/ref/#state.EditorState.update) a
   [transaction](https://codemirror.net/6/docs/ref/#state.Transaction), which produces a _new_ state
   instance, without modifying the original object.

   As such, _never_ mutate properties of a state directly. That'll
   just break things.
   */
   class EditorState {
       constructor(
       /**
       @internal
       */
       config, 
       /**
       The current document.
       */
       doc, 
       /**
       The current selection.
       */
       selection, 
       /**
       @internal
       */
       values, computeSlot, tr) {
           this.config = config;
           this.doc = doc;
           this.selection = selection;
           this.values = values;
           this.status = config.statusTemplate.slice();
           this.computeSlot = computeSlot;
           // Fill in the computed state immediately, so that further queries
           // for it made during the update return this state
           if (tr)
               tr._state = this;
           for (let i = 0; i < this.config.dynamicSlots.length; i++)
               ensureAddr(this, i << 1);
           this.computeSlot = null;
       }
       field(field, require = true) {
           let addr = this.config.address[field.id];
           if (addr == null) {
               if (require)
                   throw new RangeError("Field is not present in this state");
               return undefined;
           }
           ensureAddr(this, addr);
           return getAddr(this, addr);
       }
       /**
       Create a [transaction](https://codemirror.net/6/docs/ref/#state.Transaction) that updates this
       state. Any number of [transaction specs](https://codemirror.net/6/docs/ref/#state.TransactionSpec)
       can be passed. Unless
       [`sequential`](https://codemirror.net/6/docs/ref/#state.TransactionSpec.sequential) is set, the
       [changes](https://codemirror.net/6/docs/ref/#state.TransactionSpec.changes) (if any) of each spec
       are assumed to start in the _current_ document (not the document
       produced by previous specs), and its
       [selection](https://codemirror.net/6/docs/ref/#state.TransactionSpec.selection) and
       [effects](https://codemirror.net/6/docs/ref/#state.TransactionSpec.effects) are assumed to refer
       to the document created by its _own_ changes. The resulting
       transaction contains the combined effect of all the different
       specs. For [selection](https://codemirror.net/6/docs/ref/#state.TransactionSpec.selection), later
       specs take precedence over earlier ones.
       */
       update(...specs) {
           return resolveTransaction(this, specs, true);
       }
       /**
       @internal
       */
       applyTransaction(tr) {
           let conf = this.config, { base, compartments } = conf;
           for (let effect of tr.effects) {
               if (effect.is(Compartment.reconfigure)) {
                   if (conf) {
                       compartments = new Map;
                       conf.compartments.forEach((val, key) => compartments.set(key, val));
                       conf = null;
                   }
                   compartments.set(effect.value.compartment, effect.value.extension);
               }
               else if (effect.is(StateEffect.reconfigure)) {
                   conf = null;
                   base = effect.value;
               }
               else if (effect.is(StateEffect.appendConfig)) {
                   conf = null;
                   base = asArray(base).concat(effect.value);
               }
           }
           let startValues;
           if (!conf) {
               conf = Configuration.resolve(base, compartments, this);
               let intermediateState = new EditorState(conf, this.doc, this.selection, conf.dynamicSlots.map(() => null), (state, slot) => slot.reconfigure(state, this), null);
               startValues = intermediateState.values;
           }
           else {
               startValues = tr.startState.values.slice();
           }
           new EditorState(conf, tr.newDoc, tr.newSelection, startValues, (state, slot) => slot.update(state, tr), tr);
       }
       /**
       Create a [transaction spec](https://codemirror.net/6/docs/ref/#state.TransactionSpec) that
       replaces every selection range with the given content.
       */
       replaceSelection(text) {
           if (typeof text == "string")
               text = this.toText(text);
           return this.changeByRange(range => ({ changes: { from: range.from, to: range.to, insert: text },
               range: EditorSelection.cursor(range.from + text.length) }));
       }
       /**
       Create a set of changes and a new selection by running the given
       function for each range in the active selection. The function
       can return an optional set of changes (in the coordinate space
       of the start document), plus an updated range (in the coordinate
       space of the document produced by the call's own changes). This
       method will merge all the changes and ranges into a single
       changeset and selection, and return it as a [transaction
       spec](https://codemirror.net/6/docs/ref/#state.TransactionSpec), which can be passed to
       [`update`](https://codemirror.net/6/docs/ref/#state.EditorState.update).
       */
       changeByRange(f) {
           let sel = this.selection;
           let result1 = f(sel.ranges[0]);
           let changes = this.changes(result1.changes), ranges = [result1.range];
           let effects = asArray(result1.effects);
           for (let i = 1; i < sel.ranges.length; i++) {
               let result = f(sel.ranges[i]);
               let newChanges = this.changes(result.changes), newMapped = newChanges.map(changes);
               for (let j = 0; j < i; j++)
                   ranges[j] = ranges[j].map(newMapped);
               let mapBy = changes.mapDesc(newChanges, true);
               ranges.push(result.range.map(mapBy));
               changes = changes.compose(newMapped);
               effects = StateEffect.mapEffects(effects, newMapped).concat(StateEffect.mapEffects(asArray(result.effects), mapBy));
           }
           return {
               changes,
               selection: EditorSelection.create(ranges, sel.mainIndex),
               effects
           };
       }
       /**
       Create a [change set](https://codemirror.net/6/docs/ref/#state.ChangeSet) from the given change
       description, taking the state's document length and line
       separator into account.
       */
       changes(spec = []) {
           if (spec instanceof ChangeSet)
               return spec;
           return ChangeSet.of(spec, this.doc.length, this.facet(EditorState.lineSeparator));
       }
       /**
       Using the state's [line
       separator](https://codemirror.net/6/docs/ref/#state.EditorState^lineSeparator), create a
       [`Text`](https://codemirror.net/6/docs/ref/#state.Text) instance from the given string.
       */
       toText(string) {
           return Text.of(string.split(this.facet(EditorState.lineSeparator) || DefaultSplit));
       }
       /**
       Return the given range of the document as a string.
       */
       sliceDoc(from = 0, to = this.doc.length) {
           return this.doc.sliceString(from, to, this.lineBreak);
       }
       /**
       Get the value of a state [facet](https://codemirror.net/6/docs/ref/#state.Facet).
       */
       facet(facet) {
           let addr = this.config.address[facet.id];
           if (addr == null)
               return facet.default;
           ensureAddr(this, addr);
           return getAddr(this, addr);
       }
       /**
       Convert this state to a JSON-serializable object. When custom
       fields should be serialized, you can pass them in as an object
       mapping property names (in the resulting object, which should
       not use `doc` or `selection`) to fields.
       */
       toJSON(fields) {
           let result = {
               doc: this.sliceDoc(),
               selection: this.selection.toJSON()
           };
           if (fields)
               for (let prop in fields) {
                   let value = fields[prop];
                   if (value instanceof StateField && this.config.address[value.id] != null)
                       result[prop] = value.spec.toJSON(this.field(fields[prop]), this);
               }
           return result;
       }
       /**
       Deserialize a state from its JSON representation. When custom
       fields should be deserialized, pass the same object you passed
       to [`toJSON`](https://codemirror.net/6/docs/ref/#state.EditorState.toJSON) when serializing as
       third argument.
       */
       static fromJSON(json, config = {}, fields) {
           if (!json || typeof json.doc != "string")
               throw new RangeError("Invalid JSON representation for EditorState");
           let fieldInit = [];
           if (fields)
               for (let prop in fields) {
                   if (Object.prototype.hasOwnProperty.call(json, prop)) {
                       let field = fields[prop], value = json[prop];
                       fieldInit.push(field.init(state => field.spec.fromJSON(value, state)));
                   }
               }
           return EditorState.create({
               doc: json.doc,
               selection: EditorSelection.fromJSON(json.selection),
               extensions: config.extensions ? fieldInit.concat([config.extensions]) : fieldInit
           });
       }
       /**
       Create a new state. You'll usually only need this when
       initializing an editor—updated states are created by applying
       transactions.
       */
       static create(config = {}) {
           let configuration = Configuration.resolve(config.extensions || [], new Map);
           let doc = config.doc instanceof Text ? config.doc
               : Text.of((config.doc || "").split(configuration.staticFacet(EditorState.lineSeparator) || DefaultSplit));
           let selection = !config.selection ? EditorSelection.single(0)
               : config.selection instanceof EditorSelection ? config.selection
                   : EditorSelection.single(config.selection.anchor, config.selection.head);
           checkSelection(selection, doc.length);
           if (!configuration.staticFacet(allowMultipleSelections))
               selection = selection.asSingle();
           return new EditorState(configuration, doc, selection, configuration.dynamicSlots.map(() => null), (state, slot) => slot.create(state), null);
       }
       /**
       The size (in columns) of a tab in the document, determined by
       the [`tabSize`](https://codemirror.net/6/docs/ref/#state.EditorState^tabSize) facet.
       */
       get tabSize() { return this.facet(EditorState.tabSize); }
       /**
       Get the proper [line-break](https://codemirror.net/6/docs/ref/#state.EditorState^lineSeparator)
       string for this state.
       */
       get lineBreak() { return this.facet(EditorState.lineSeparator) || "\n"; }
       /**
       Returns true when the editor is
       [configured](https://codemirror.net/6/docs/ref/#state.EditorState^readOnly) to be read-only.
       */
       get readOnly() { return this.facet(readOnly); }
       /**
       Look up a translation for the given phrase (via the
       [`phrases`](https://codemirror.net/6/docs/ref/#state.EditorState^phrases) facet), or return the
       original string if no translation is found.
       
       If additional arguments are passed, they will be inserted in
       place of markers like `$1` (for the first value) and `$2`, etc.
       A single `$` is equivalent to `$1`, and `$$` will produce a
       literal dollar sign.
       */
       phrase(phrase, ...insert) {
           for (let map of this.facet(EditorState.phrases))
               if (Object.prototype.hasOwnProperty.call(map, phrase)) {
                   phrase = map[phrase];
                   break;
               }
           if (insert.length)
               phrase = phrase.replace(/\$(\$|\d*)/g, (m, i) => {
                   if (i == "$")
                       return "$";
                   let n = +(i || 1);
                   return !n || n > insert.length ? m : insert[n - 1];
               });
           return phrase;
       }
       /**
       Find the values for a given language data field, provided by the
       the [`languageData`](https://codemirror.net/6/docs/ref/#state.EditorState^languageData) facet.
       */
       languageDataAt(name, pos, side = -1) {
           let values = [];
           for (let provider of this.facet(languageData)) {
               for (let result of provider(this, pos, side)) {
                   if (Object.prototype.hasOwnProperty.call(result, name))
                       values.push(result[name]);
               }
           }
           return values;
       }
       /**
       Return a function that can categorize strings (expected to
       represent a single [grapheme cluster](https://codemirror.net/6/docs/ref/#state.findClusterBreak))
       into one of:
       
        - Word (contains an alphanumeric character or a character
          explicitly listed in the local language's `"wordChars"`
          language data, which should be a string)
        - Space (contains only whitespace)
        - Other (anything else)
       */
       charCategorizer(at) {
           return makeCategorizer(this.languageDataAt("wordChars", at).join(""));
       }
       /**
       Find the word at the given position, meaning the range
       containing all [word](https://codemirror.net/6/docs/ref/#state.CharCategory.Word) characters
       around it. If no word characters are adjacent to the position,
       this returns null.
       */
       wordAt(pos) {
           let { text, from, length } = this.doc.lineAt(pos);
           let cat = this.charCategorizer(pos);
           let start = pos - from, end = pos - from;
           while (start > 0) {
               let prev = findClusterBreak(text, start, false);
               if (cat(text.slice(prev, start)) != CharCategory.Word)
                   break;
               start = prev;
           }
           while (end < length) {
               let next = findClusterBreak(text, end);
               if (cat(text.slice(end, next)) != CharCategory.Word)
                   break;
               end = next;
           }
           return start == end ? null : EditorSelection.range(start + from, end + from);
       }
   }
   /**
   A facet that, when enabled, causes the editor to allow multiple
   ranges to be selected. Be careful though, because by default the
   editor relies on the native DOM selection, which cannot handle
   multiple selections. An extension like
   [`drawSelection`](https://codemirror.net/6/docs/ref/#view.drawSelection) can be used to make
   secondary selections visible to the user.
   */
   EditorState.allowMultipleSelections = allowMultipleSelections;
   /**
   Configures the tab size to use in this state. The first
   (highest-precedence) value of the facet is used. If no value is
   given, this defaults to 4.
   */
   EditorState.tabSize = /*@__PURE__*/Facet.define({
       combine: values => values.length ? values[0] : 4
   });
   /**
   The line separator to use. By default, any of `"\n"`, `"\r\n"`
   and `"\r"` is treated as a separator when splitting lines, and
   lines are joined with `"\n"`.

   When you configure a value here, only that precise separator
   will be used, allowing you to round-trip documents through the
   editor without normalizing line separators.
   */
   EditorState.lineSeparator = lineSeparator;
   /**
   This facet controls the value of the
   [`readOnly`](https://codemirror.net/6/docs/ref/#state.EditorState.readOnly) getter, which is
   consulted by commands and extensions that implement editing
   functionality to determine whether they should apply. It
   defaults to false, but when its highest-precedence value is
   `true`, such functionality disables itself.

   Not to be confused with
   [`EditorView.editable`](https://codemirror.net/6/docs/ref/#view.EditorView^editable), which
   controls whether the editor's DOM is set to be editable (and
   thus focusable).
   */
   EditorState.readOnly = readOnly;
   /**
   Registers translation phrases. The
   [`phrase`](https://codemirror.net/6/docs/ref/#state.EditorState.phrase) method will look through
   all objects registered with this facet to find translations for
   its argument.
   */
   EditorState.phrases = /*@__PURE__*/Facet.define({
       compare(a, b) {
           let kA = Object.keys(a), kB = Object.keys(b);
           return kA.length == kB.length && kA.every(k => a[k] == b[k]);
       }
   });
   /**
   A facet used to register [language
   data](https://codemirror.net/6/docs/ref/#state.EditorState.languageDataAt) providers.
   */
   EditorState.languageData = languageData;
   /**
   Facet used to register change filters, which are called for each
   transaction (unless explicitly
   [disabled](https://codemirror.net/6/docs/ref/#state.TransactionSpec.filter)), and can suppress
   part of the transaction's changes.

   Such a function can return `true` to indicate that it doesn't
   want to do anything, `false` to completely stop the changes in
   the transaction, or a set of ranges in which changes should be
   suppressed. Such ranges are represented as an array of numbers,
   with each pair of two numbers indicating the start and end of a
   range. So for example `[10, 20, 100, 110]` suppresses changes
   between 10 and 20, and between 100 and 110.
   */
   EditorState.changeFilter = changeFilter;
   /**
   Facet used to register a hook that gets a chance to update or
   replace transaction specs before they are applied. This will
   only be applied for transactions that don't have
   [`filter`](https://codemirror.net/6/docs/ref/#state.TransactionSpec.filter) set to `false`. You
   can either return a single transaction spec (possibly the input
   transaction), or an array of specs (which will be combined in
   the same way as the arguments to
   [`EditorState.update`](https://codemirror.net/6/docs/ref/#state.EditorState.update)).

   When possible, it is recommended to avoid accessing
   [`Transaction.state`](https://codemirror.net/6/docs/ref/#state.Transaction.state) in a filter,
   since it will force creation of a state that will then be
   discarded again, if the transaction is actually filtered.

   (This functionality should be used with care. Indiscriminately
   modifying transaction is likely to break something or degrade
   the user experience.)
   */
   EditorState.transactionFilter = transactionFilter;
   /**
   This is a more limited form of
   [`transactionFilter`](https://codemirror.net/6/docs/ref/#state.EditorState^transactionFilter),
   which can only add
   [annotations](https://codemirror.net/6/docs/ref/#state.TransactionSpec.annotations) and
   [effects](https://codemirror.net/6/docs/ref/#state.TransactionSpec.effects). _But_, this type
   of filter runs even if the transaction has disabled regular
   [filtering](https://codemirror.net/6/docs/ref/#state.TransactionSpec.filter), making it suitable
   for effects that don't need to touch the changes or selection,
   but do want to process every transaction.

   Extenders run _after_ filters, when both are present.
   */
   EditorState.transactionExtender = transactionExtender;
   Compartment.reconfigure = /*@__PURE__*/StateEffect.define();

   /**
   Utility function for combining behaviors to fill in a config
   object from an array of provided configs. `defaults` should hold
   default values for all optional fields in `Config`.

   The function will, by default, error
   when a field gets two values that aren't `===`-equal, but you can
   provide combine functions per field to do something else.
   */
   function combineConfig(configs, defaults, // Should hold only the optional properties of Config, but I haven't managed to express that
   combine = {}) {
       let result = {};
       for (let config of configs)
           for (let key of Object.keys(config)) {
               let value = config[key], current = result[key];
               if (current === undefined)
                   result[key] = value;
               else if (current === value || value === undefined) ; // No conflict
               else if (Object.hasOwnProperty.call(combine, key))
                   result[key] = combine[key](current, value);
               else
                   throw new Error("Config merge conflict for field " + key);
           }
       for (let key in defaults)
           if (result[key] === undefined)
               result[key] = defaults[key];
       return result;
   }

   /**
   Each range is associated with a value, which must inherit from
   this class.
   */
   class RangeValue {
       /**
       Compare this value with another value. Used when comparing
       rangesets. The default implementation compares by identity.
       Unless you are only creating a fixed number of unique instances
       of your value type, it is a good idea to implement this
       properly.
       */
       eq(other) { return this == other; }
       /**
       Create a [range](https://codemirror.net/6/docs/ref/#state.Range) with this value.
       */
       range(from, to = from) { return Range$1.create(from, to, this); }
   }
   RangeValue.prototype.startSide = RangeValue.prototype.endSide = 0;
   RangeValue.prototype.point = false;
   RangeValue.prototype.mapMode = MapMode.TrackDel;
   /**
   A range associates a value with a range of positions.
   */
   class Range$1 {
       constructor(
       /**
       The range's start position.
       */
       from, 
       /**
       Its end position.
       */
       to, 
       /**
       The value associated with this range.
       */
       value) {
           this.from = from;
           this.to = to;
           this.value = value;
       }
       /**
       @internal
       */
       static create(from, to, value) {
           return new Range$1(from, to, value);
       }
   }
   function cmpRange(a, b) {
       return a.from - b.from || a.value.startSide - b.value.startSide;
   }
   class Chunk {
       constructor(from, to, value, 
       // Chunks are marked with the largest point that occurs
       // in them (or -1 for no points), so that scans that are
       // only interested in points (such as the
       // heightmap-related logic) can skip range-only chunks.
       maxPoint) {
           this.from = from;
           this.to = to;
           this.value = value;
           this.maxPoint = maxPoint;
       }
       get length() { return this.to[this.to.length - 1]; }
       // Find the index of the given position and side. Use the ranges'
       // `from` pos when `end == false`, `to` when `end == true`.
       findIndex(pos, side, end, startAt = 0) {
           let arr = end ? this.to : this.from;
           for (let lo = startAt, hi = arr.length;;) {
               if (lo == hi)
                   return lo;
               let mid = (lo + hi) >> 1;
               let diff = arr[mid] - pos || (end ? this.value[mid].endSide : this.value[mid].startSide) - side;
               if (mid == lo)
                   return diff >= 0 ? lo : hi;
               if (diff >= 0)
                   hi = mid;
               else
                   lo = mid + 1;
           }
       }
       between(offset, from, to, f) {
           for (let i = this.findIndex(from, -1000000000 /* Far */, true), e = this.findIndex(to, 1000000000 /* Far */, false, i); i < e; i++)
               if (f(this.from[i] + offset, this.to[i] + offset, this.value[i]) === false)
                   return false;
       }
       map(offset, changes) {
           let value = [], from = [], to = [], newPos = -1, maxPoint = -1;
           for (let i = 0; i < this.value.length; i++) {
               let val = this.value[i], curFrom = this.from[i] + offset, curTo = this.to[i] + offset, newFrom, newTo;
               if (curFrom == curTo) {
                   let mapped = changes.mapPos(curFrom, val.startSide, val.mapMode);
                   if (mapped == null)
                       continue;
                   newFrom = newTo = mapped;
                   if (val.startSide != val.endSide) {
                       newTo = changes.mapPos(curFrom, val.endSide);
                       if (newTo < newFrom)
                           continue;
                   }
               }
               else {
                   newFrom = changes.mapPos(curFrom, val.startSide);
                   newTo = changes.mapPos(curTo, val.endSide);
                   if (newFrom > newTo || newFrom == newTo && val.startSide > 0 && val.endSide <= 0)
                       continue;
               }
               if ((newTo - newFrom || val.endSide - val.startSide) < 0)
                   continue;
               if (newPos < 0)
                   newPos = newFrom;
               if (val.point)
                   maxPoint = Math.max(maxPoint, newTo - newFrom);
               value.push(val);
               from.push(newFrom - newPos);
               to.push(newTo - newPos);
           }
           return { mapped: value.length ? new Chunk(from, to, value, maxPoint) : null, pos: newPos };
       }
   }
   /**
   A range set stores a collection of [ranges](https://codemirror.net/6/docs/ref/#state.Range) in a
   way that makes them efficient to [map](https://codemirror.net/6/docs/ref/#state.RangeSet.map) and
   [update](https://codemirror.net/6/docs/ref/#state.RangeSet.update). This is an immutable data
   structure.
   */
   class RangeSet {
       constructor(
       /**
       @internal
       */
       chunkPos, 
       /**
       @internal
       */
       chunk, 
       /**
       @internal
       */
       nextLayer, 
       /**
       @internal
       */
       maxPoint) {
           this.chunkPos = chunkPos;
           this.chunk = chunk;
           this.nextLayer = nextLayer;
           this.maxPoint = maxPoint;
       }
       /**
       @internal
       */
       static create(chunkPos, chunk, nextLayer, maxPoint) {
           return new RangeSet(chunkPos, chunk, nextLayer, maxPoint);
       }
       /**
       @internal
       */
       get length() {
           let last = this.chunk.length - 1;
           return last < 0 ? 0 : Math.max(this.chunkEnd(last), this.nextLayer.length);
       }
       /**
       The number of ranges in the set.
       */
       get size() {
           if (this.isEmpty)
               return 0;
           let size = this.nextLayer.size;
           for (let chunk of this.chunk)
               size += chunk.value.length;
           return size;
       }
       /**
       @internal
       */
       chunkEnd(index) {
           return this.chunkPos[index] + this.chunk[index].length;
       }
       /**
       Update the range set, optionally adding new ranges or filtering
       out existing ones.
       
       (Note: The type parameter is just there as a kludge to work
       around TypeScript variance issues that prevented `RangeSet<X>`
       from being a subtype of `RangeSet<Y>` when `X` is a subtype of
       `Y`.)
       */
       update(updateSpec) {
           let { add = [], sort = false, filterFrom = 0, filterTo = this.length } = updateSpec;
           let filter = updateSpec.filter;
           if (add.length == 0 && !filter)
               return this;
           if (sort)
               add = add.slice().sort(cmpRange);
           if (this.isEmpty)
               return add.length ? RangeSet.of(add) : this;
           let cur = new LayerCursor(this, null, -1).goto(0), i = 0, spill = [];
           let builder = new RangeSetBuilder();
           while (cur.value || i < add.length) {
               if (i < add.length && (cur.from - add[i].from || cur.startSide - add[i].value.startSide) >= 0) {
                   let range = add[i++];
                   if (!builder.addInner(range.from, range.to, range.value))
                       spill.push(range);
               }
               else if (cur.rangeIndex == 1 && cur.chunkIndex < this.chunk.length &&
                   (i == add.length || this.chunkEnd(cur.chunkIndex) < add[i].from) &&
                   (!filter || filterFrom > this.chunkEnd(cur.chunkIndex) || filterTo < this.chunkPos[cur.chunkIndex]) &&
                   builder.addChunk(this.chunkPos[cur.chunkIndex], this.chunk[cur.chunkIndex])) {
                   cur.nextChunk();
               }
               else {
                   if (!filter || filterFrom > cur.to || filterTo < cur.from || filter(cur.from, cur.to, cur.value)) {
                       if (!builder.addInner(cur.from, cur.to, cur.value))
                           spill.push(Range$1.create(cur.from, cur.to, cur.value));
                   }
                   cur.next();
               }
           }
           return builder.finishInner(this.nextLayer.isEmpty && !spill.length ? RangeSet.empty
               : this.nextLayer.update({ add: spill, filter, filterFrom, filterTo }));
       }
       /**
       Map this range set through a set of changes, return the new set.
       */
       map(changes) {
           if (changes.empty || this.isEmpty)
               return this;
           let chunks = [], chunkPos = [], maxPoint = -1;
           for (let i = 0; i < this.chunk.length; i++) {
               let start = this.chunkPos[i], chunk = this.chunk[i];
               let touch = changes.touchesRange(start, start + chunk.length);
               if (touch === false) {
                   maxPoint = Math.max(maxPoint, chunk.maxPoint);
                   chunks.push(chunk);
                   chunkPos.push(changes.mapPos(start));
               }
               else if (touch === true) {
                   let { mapped, pos } = chunk.map(start, changes);
                   if (mapped) {
                       maxPoint = Math.max(maxPoint, mapped.maxPoint);
                       chunks.push(mapped);
                       chunkPos.push(pos);
                   }
               }
           }
           let next = this.nextLayer.map(changes);
           return chunks.length == 0 ? next : new RangeSet(chunkPos, chunks, next || RangeSet.empty, maxPoint);
       }
       /**
       Iterate over the ranges that touch the region `from` to `to`,
       calling `f` for each. There is no guarantee that the ranges will
       be reported in any specific order. When the callback returns
       `false`, iteration stops.
       */
       between(from, to, f) {
           if (this.isEmpty)
               return;
           for (let i = 0; i < this.chunk.length; i++) {
               let start = this.chunkPos[i], chunk = this.chunk[i];
               if (to >= start && from <= start + chunk.length &&
                   chunk.between(start, from - start, to - start, f) === false)
                   return;
           }
           this.nextLayer.between(from, to, f);
       }
       /**
       Iterate over the ranges in this set, in order, including all
       ranges that end at or after `from`.
       */
       iter(from = 0) {
           return HeapCursor.from([this]).goto(from);
       }
       /**
       @internal
       */
       get isEmpty() { return this.nextLayer == this; }
       /**
       Iterate over the ranges in a collection of sets, in order,
       starting from `from`.
       */
       static iter(sets, from = 0) {
           return HeapCursor.from(sets).goto(from);
       }
       /**
       Iterate over two groups of sets, calling methods on `comparator`
       to notify it of possible differences.
       */
       static compare(oldSets, newSets, 
       /**
       This indicates how the underlying data changed between these
       ranges, and is needed to synchronize the iteration. `from` and
       `to` are coordinates in the _new_ space, after these changes.
       */
       textDiff, comparator, 
       /**
       Can be used to ignore all non-point ranges, and points below
       the given size. When -1, all ranges are compared.
       */
       minPointSize = -1) {
           let a = oldSets.filter(set => set.maxPoint > 0 || !set.isEmpty && set.maxPoint >= minPointSize);
           let b = newSets.filter(set => set.maxPoint > 0 || !set.isEmpty && set.maxPoint >= minPointSize);
           let sharedChunks = findSharedChunks(a, b, textDiff);
           let sideA = new SpanCursor(a, sharedChunks, minPointSize);
           let sideB = new SpanCursor(b, sharedChunks, minPointSize);
           textDiff.iterGaps((fromA, fromB, length) => compare(sideA, fromA, sideB, fromB, length, comparator));
           if (textDiff.empty && textDiff.length == 0)
               compare(sideA, 0, sideB, 0, 0, comparator);
       }
       /**
       Compare the contents of two groups of range sets, returning true
       if they are equivalent in the given range.
       */
       static eq(oldSets, newSets, from = 0, to) {
           if (to == null)
               to = 1000000000 /* Far */;
           let a = oldSets.filter(set => !set.isEmpty && newSets.indexOf(set) < 0);
           let b = newSets.filter(set => !set.isEmpty && oldSets.indexOf(set) < 0);
           if (a.length != b.length)
               return false;
           if (!a.length)
               return true;
           let sharedChunks = findSharedChunks(a, b);
           let sideA = new SpanCursor(a, sharedChunks, 0).goto(from), sideB = new SpanCursor(b, sharedChunks, 0).goto(from);
           for (;;) {
               if (sideA.to != sideB.to ||
                   !sameValues(sideA.active, sideB.active) ||
                   sideA.point && (!sideB.point || !sideA.point.eq(sideB.point)))
                   return false;
               if (sideA.to > to)
                   return true;
               sideA.next();
               sideB.next();
           }
       }
       /**
       Iterate over a group of range sets at the same time, notifying
       the iterator about the ranges covering every given piece of
       content. Returns the open count (see
       [`SpanIterator.span`](https://codemirror.net/6/docs/ref/#state.SpanIterator.span)) at the end
       of the iteration.
       */
       static spans(sets, from, to, iterator, 
       /**
       When given and greater than -1, only points of at least this
       size are taken into account.
       */
       minPointSize = -1) {
           let cursor = new SpanCursor(sets, null, minPointSize).goto(from), pos = from;
           let open = cursor.openStart;
           for (;;) {
               let curTo = Math.min(cursor.to, to);
               if (cursor.point) {
                   iterator.point(pos, curTo, cursor.point, cursor.activeForPoint(cursor.to), open, cursor.pointRank);
                   open = cursor.openEnd(curTo) + (cursor.to > curTo ? 1 : 0);
               }
               else if (curTo > pos) {
                   iterator.span(pos, curTo, cursor.active, open);
                   open = cursor.openEnd(curTo);
               }
               if (cursor.to > to)
                   break;
               pos = cursor.to;
               cursor.next();
           }
           return open;
       }
       /**
       Create a range set for the given range or array of ranges. By
       default, this expects the ranges to be _sorted_ (by start
       position and, if two start at the same position,
       `value.startSide`). You can pass `true` as second argument to
       cause the method to sort them.
       */
       static of(ranges, sort = false) {
           let build = new RangeSetBuilder();
           for (let range of ranges instanceof Range$1 ? [ranges] : sort ? lazySort(ranges) : ranges)
               build.add(range.from, range.to, range.value);
           return build.finish();
       }
   }
   /**
   The empty set of ranges.
   */
   RangeSet.empty = /*@__PURE__*/new RangeSet([], [], null, -1);
   function lazySort(ranges) {
       if (ranges.length > 1)
           for (let prev = ranges[0], i = 1; i < ranges.length; i++) {
               let cur = ranges[i];
               if (cmpRange(prev, cur) > 0)
                   return ranges.slice().sort(cmpRange);
               prev = cur;
           }
       return ranges;
   }
   RangeSet.empty.nextLayer = RangeSet.empty;
   /**
   A range set builder is a data structure that helps build up a
   [range set](https://codemirror.net/6/docs/ref/#state.RangeSet) directly, without first allocating
   an array of [`Range`](https://codemirror.net/6/docs/ref/#state.Range) objects.
   */
   class RangeSetBuilder {
       /**
       Create an empty builder.
       */
       constructor() {
           this.chunks = [];
           this.chunkPos = [];
           this.chunkStart = -1;
           this.last = null;
           this.lastFrom = -1000000000 /* Far */;
           this.lastTo = -1000000000 /* Far */;
           this.from = [];
           this.to = [];
           this.value = [];
           this.maxPoint = -1;
           this.setMaxPoint = -1;
           this.nextLayer = null;
       }
       finishChunk(newArrays) {
           this.chunks.push(new Chunk(this.from, this.to, this.value, this.maxPoint));
           this.chunkPos.push(this.chunkStart);
           this.chunkStart = -1;
           this.setMaxPoint = Math.max(this.setMaxPoint, this.maxPoint);
           this.maxPoint = -1;
           if (newArrays) {
               this.from = [];
               this.to = [];
               this.value = [];
           }
       }
       /**
       Add a range. Ranges should be added in sorted (by `from` and
       `value.startSide`) order.
       */
       add(from, to, value) {
           if (!this.addInner(from, to, value))
               (this.nextLayer || (this.nextLayer = new RangeSetBuilder)).add(from, to, value);
       }
       /**
       @internal
       */
       addInner(from, to, value) {
           let diff = from - this.lastTo || value.startSide - this.last.endSide;
           if (diff <= 0 && (from - this.lastFrom || value.startSide - this.last.startSide) < 0)
               throw new Error("Ranges must be added sorted by `from` position and `startSide`");
           if (diff < 0)
               return false;
           if (this.from.length == 250 /* ChunkSize */)
               this.finishChunk(true);
           if (this.chunkStart < 0)
               this.chunkStart = from;
           this.from.push(from - this.chunkStart);
           this.to.push(to - this.chunkStart);
           this.last = value;
           this.lastFrom = from;
           this.lastTo = to;
           this.value.push(value);
           if (value.point)
               this.maxPoint = Math.max(this.maxPoint, to - from);
           return true;
       }
       /**
       @internal
       */
       addChunk(from, chunk) {
           if ((from - this.lastTo || chunk.value[0].startSide - this.last.endSide) < 0)
               return false;
           if (this.from.length)
               this.finishChunk(true);
           this.setMaxPoint = Math.max(this.setMaxPoint, chunk.maxPoint);
           this.chunks.push(chunk);
           this.chunkPos.push(from);
           let last = chunk.value.length - 1;
           this.last = chunk.value[last];
           this.lastFrom = chunk.from[last] + from;
           this.lastTo = chunk.to[last] + from;
           return true;
       }
       /**
       Finish the range set. Returns the new set. The builder can't be
       used anymore after this has been called.
       */
       finish() { return this.finishInner(RangeSet.empty); }
       /**
       @internal
       */
       finishInner(next) {
           if (this.from.length)
               this.finishChunk(false);
           if (this.chunks.length == 0)
               return next;
           let result = RangeSet.create(this.chunkPos, this.chunks, this.nextLayer ? this.nextLayer.finishInner(next) : next, this.setMaxPoint);
           this.from = null; // Make sure further `add` calls produce errors
           return result;
       }
   }
   function findSharedChunks(a, b, textDiff) {
       let inA = new Map();
       for (let set of a)
           for (let i = 0; i < set.chunk.length; i++)
               if (set.chunk[i].maxPoint <= 0)
                   inA.set(set.chunk[i], set.chunkPos[i]);
       let shared = new Set();
       for (let set of b)
           for (let i = 0; i < set.chunk.length; i++) {
               let known = inA.get(set.chunk[i]);
               if (known != null && (textDiff ? textDiff.mapPos(known) : known) == set.chunkPos[i] &&
                   !(textDiff === null || textDiff === void 0 ? void 0 : textDiff.touchesRange(known, known + set.chunk[i].length)))
                   shared.add(set.chunk[i]);
           }
       return shared;
   }
   class LayerCursor {
       constructor(layer, skip, minPoint, rank = 0) {
           this.layer = layer;
           this.skip = skip;
           this.minPoint = minPoint;
           this.rank = rank;
       }
       get startSide() { return this.value ? this.value.startSide : 0; }
       get endSide() { return this.value ? this.value.endSide : 0; }
       goto(pos, side = -1000000000 /* Far */) {
           this.chunkIndex = this.rangeIndex = 0;
           this.gotoInner(pos, side, false);
           return this;
       }
       gotoInner(pos, side, forward) {
           while (this.chunkIndex < this.layer.chunk.length) {
               let next = this.layer.chunk[this.chunkIndex];
               if (!(this.skip && this.skip.has(next) ||
                   this.layer.chunkEnd(this.chunkIndex) < pos ||
                   next.maxPoint < this.minPoint))
                   break;
               this.chunkIndex++;
               forward = false;
           }
           if (this.chunkIndex < this.layer.chunk.length) {
               let rangeIndex = this.layer.chunk[this.chunkIndex].findIndex(pos - this.layer.chunkPos[this.chunkIndex], side, true);
               if (!forward || this.rangeIndex < rangeIndex)
                   this.setRangeIndex(rangeIndex);
           }
           this.next();
       }
       forward(pos, side) {
           if ((this.to - pos || this.endSide - side) < 0)
               this.gotoInner(pos, side, true);
       }
       next() {
           for (;;) {
               if (this.chunkIndex == this.layer.chunk.length) {
                   this.from = this.to = 1000000000 /* Far */;
                   this.value = null;
                   break;
               }
               else {
                   let chunkPos = this.layer.chunkPos[this.chunkIndex], chunk = this.layer.chunk[this.chunkIndex];
                   let from = chunkPos + chunk.from[this.rangeIndex];
                   this.from = from;
                   this.to = chunkPos + chunk.to[this.rangeIndex];
                   this.value = chunk.value[this.rangeIndex];
                   this.setRangeIndex(this.rangeIndex + 1);
                   if (this.minPoint < 0 || this.value.point && this.to - this.from >= this.minPoint)
                       break;
               }
           }
       }
       setRangeIndex(index) {
           if (index == this.layer.chunk[this.chunkIndex].value.length) {
               this.chunkIndex++;
               if (this.skip) {
                   while (this.chunkIndex < this.layer.chunk.length && this.skip.has(this.layer.chunk[this.chunkIndex]))
                       this.chunkIndex++;
               }
               this.rangeIndex = 0;
           }
           else {
               this.rangeIndex = index;
           }
       }
       nextChunk() {
           this.chunkIndex++;
           this.rangeIndex = 0;
           this.next();
       }
       compare(other) {
           return this.from - other.from || this.startSide - other.startSide || this.rank - other.rank ||
               this.to - other.to || this.endSide - other.endSide;
       }
   }
   class HeapCursor {
       constructor(heap) {
           this.heap = heap;
       }
       static from(sets, skip = null, minPoint = -1) {
           let heap = [];
           for (let i = 0; i < sets.length; i++) {
               for (let cur = sets[i]; !cur.isEmpty; cur = cur.nextLayer) {
                   if (cur.maxPoint >= minPoint)
                       heap.push(new LayerCursor(cur, skip, minPoint, i));
               }
           }
           return heap.length == 1 ? heap[0] : new HeapCursor(heap);
       }
       get startSide() { return this.value ? this.value.startSide : 0; }
       goto(pos, side = -1000000000 /* Far */) {
           for (let cur of this.heap)
               cur.goto(pos, side);
           for (let i = this.heap.length >> 1; i >= 0; i--)
               heapBubble(this.heap, i);
           this.next();
           return this;
       }
       forward(pos, side) {
           for (let cur of this.heap)
               cur.forward(pos, side);
           for (let i = this.heap.length >> 1; i >= 0; i--)
               heapBubble(this.heap, i);
           if ((this.to - pos || this.value.endSide - side) < 0)
               this.next();
       }
       next() {
           if (this.heap.length == 0) {
               this.from = this.to = 1000000000 /* Far */;
               this.value = null;
               this.rank = -1;
           }
           else {
               let top = this.heap[0];
               this.from = top.from;
               this.to = top.to;
               this.value = top.value;
               this.rank = top.rank;
               if (top.value)
                   top.next();
               heapBubble(this.heap, 0);
           }
       }
   }
   function heapBubble(heap, index) {
       for (let cur = heap[index];;) {
           let childIndex = (index << 1) + 1;
           if (childIndex >= heap.length)
               break;
           let child = heap[childIndex];
           if (childIndex + 1 < heap.length && child.compare(heap[childIndex + 1]) >= 0) {
               child = heap[childIndex + 1];
               childIndex++;
           }
           if (cur.compare(child) < 0)
               break;
           heap[childIndex] = cur;
           heap[index] = child;
           index = childIndex;
       }
   }
   class SpanCursor {
       constructor(sets, skip, minPoint) {
           this.minPoint = minPoint;
           this.active = [];
           this.activeTo = [];
           this.activeRank = [];
           this.minActive = -1;
           // A currently active point range, if any
           this.point = null;
           this.pointFrom = 0;
           this.pointRank = 0;
           this.to = -1000000000 /* Far */;
           this.endSide = 0;
           this.openStart = -1;
           this.cursor = HeapCursor.from(sets, skip, minPoint);
       }
       goto(pos, side = -1000000000 /* Far */) {
           this.cursor.goto(pos, side);
           this.active.length = this.activeTo.length = this.activeRank.length = 0;
           this.minActive = -1;
           this.to = pos;
           this.endSide = side;
           this.openStart = -1;
           this.next();
           return this;
       }
       forward(pos, side) {
           while (this.minActive > -1 && (this.activeTo[this.minActive] - pos || this.active[this.minActive].endSide - side) < 0)
               this.removeActive(this.minActive);
           this.cursor.forward(pos, side);
       }
       removeActive(index) {
           remove(this.active, index);
           remove(this.activeTo, index);
           remove(this.activeRank, index);
           this.minActive = findMinIndex(this.active, this.activeTo);
       }
       addActive(trackOpen) {
           let i = 0, { value, to, rank } = this.cursor;
           while (i < this.activeRank.length && this.activeRank[i] <= rank)
               i++;
           insert(this.active, i, value);
           insert(this.activeTo, i, to);
           insert(this.activeRank, i, rank);
           if (trackOpen)
               insert(trackOpen, i, this.cursor.from);
           this.minActive = findMinIndex(this.active, this.activeTo);
       }
       // After calling this, if `this.point` != null, the next range is a
       // point. Otherwise, it's a regular range, covered by `this.active`.
       next() {
           let from = this.to, wasPoint = this.point;
           this.point = null;
           let trackOpen = this.openStart < 0 ? [] : null, trackExtra = 0;
           for (;;) {
               let a = this.minActive;
               if (a > -1 && (this.activeTo[a] - this.cursor.from || this.active[a].endSide - this.cursor.startSide) < 0) {
                   if (this.activeTo[a] > from) {
                       this.to = this.activeTo[a];
                       this.endSide = this.active[a].endSide;
                       break;
                   }
                   this.removeActive(a);
                   if (trackOpen)
                       remove(trackOpen, a);
               }
               else if (!this.cursor.value) {
                   this.to = this.endSide = 1000000000 /* Far */;
                   break;
               }
               else if (this.cursor.from > from) {
                   this.to = this.cursor.from;
                   this.endSide = this.cursor.startSide;
                   break;
               }
               else {
                   let nextVal = this.cursor.value;
                   if (!nextVal.point) { // Opening a range
                       this.addActive(trackOpen);
                       if (this.cursor.from < from && this.cursor.to > from)
                           trackExtra++;
                       this.cursor.next();
                   }
                   else if (wasPoint && this.cursor.to == this.to && this.cursor.from < this.cursor.to) {
                       // Ignore any non-empty points that end precisely at the end of the prev point
                       this.cursor.next();
                   }
                   else { // New point
                       this.point = nextVal;
                       this.pointFrom = this.cursor.from;
                       this.pointRank = this.cursor.rank;
                       this.to = this.cursor.to;
                       this.endSide = nextVal.endSide;
                       if (this.cursor.from < from)
                           trackExtra = 1;
                       this.cursor.next();
                       this.forward(this.to, this.endSide);
                       break;
                   }
               }
           }
           if (trackOpen) {
               let openStart = 0;
               while (openStart < trackOpen.length && trackOpen[openStart] < from)
                   openStart++;
               this.openStart = openStart + trackExtra;
           }
       }
       activeForPoint(to) {
           if (!this.active.length)
               return this.active;
           let active = [];
           for (let i = this.active.length - 1; i >= 0; i--) {
               if (this.activeRank[i] < this.pointRank)
                   break;
               if (this.activeTo[i] > to || this.activeTo[i] == to && this.active[i].endSide >= this.point.endSide)
                   active.push(this.active[i]);
           }
           return active.reverse();
       }
       openEnd(to) {
           let open = 0;
           for (let i = this.activeTo.length - 1; i >= 0 && this.activeTo[i] > to; i--)
               open++;
           return open;
       }
   }
   function compare(a, startA, b, startB, length, comparator) {
       a.goto(startA);
       b.goto(startB);
       let endB = startB + length;
       let pos = startB, dPos = startB - startA;
       for (;;) {
           let diff = (a.to + dPos) - b.to || a.endSide - b.endSide;
           let end = diff < 0 ? a.to + dPos : b.to, clipEnd = Math.min(end, endB);
           if (a.point || b.point) {
               if (!(a.point && b.point && (a.point == b.point || a.point.eq(b.point)) &&
                   sameValues(a.activeForPoint(a.to + dPos), b.activeForPoint(b.to))))
                   comparator.comparePoint(pos, clipEnd, a.point, b.point);
           }
           else {
               if (clipEnd > pos && !sameValues(a.active, b.active))
                   comparator.compareRange(pos, clipEnd, a.active, b.active);
           }
           if (end > endB)
               break;
           pos = end;
           if (diff <= 0)
               a.next();
           if (diff >= 0)
               b.next();
       }
   }
   function sameValues(a, b) {
       if (a.length != b.length)
           return false;
       for (let i = 0; i < a.length; i++)
           if (a[i] != b[i] && !a[i].eq(b[i]))
               return false;
       return true;
   }
   function remove(array, index) {
       for (let i = index, e = array.length - 1; i < e; i++)
           array[i] = array[i + 1];
       array.pop();
   }
   function insert(array, index, value) {
       for (let i = array.length - 1; i >= index; i--)
           array[i + 1] = array[i];
       array[index] = value;
   }
   function findMinIndex(value, array) {
       let found = -1, foundPos = 1000000000 /* Far */;
       for (let i = 0; i < array.length; i++)
           if ((array[i] - foundPos || value[i].endSide - value[found].endSide) < 0) {
               found = i;
               foundPos = array[i];
           }
       return found;
   }

   /**
   Count the column position at the given offset into the string,
   taking extending characters and tab size into account.
   */
   function countColumn(string, tabSize, to = string.length) {
       let n = 0;
       for (let i = 0; i < to;) {
           if (string.charCodeAt(i) == 9) {
               n += tabSize - (n % tabSize);
               i++;
           }
           else {
               n++;
               i = findClusterBreak(string, i);
           }
       }
       return n;
   }
   /**
   Find the offset that corresponds to the given column position in a
   string, taking extending characters and tab size into account. By
   default, the string length is returned when it is too short to
   reach the column. Pass `strict` true to make it return -1 in that
   situation.
   */
   function findColumn(string, col, tabSize, strict) {
       for (let i = 0, n = 0;;) {
           if (n >= col)
               return i;
           if (i == string.length)
               break;
           n += string.charCodeAt(i) == 9 ? tabSize - (n % tabSize) : 1;
           i = findClusterBreak(string, i);
       }
       return strict === true ? -1 : string.length;
   }

   const C = "\u037c";
   const COUNT = typeof Symbol == "undefined" ? "__" + C : Symbol.for(C);
   const SET = typeof Symbol == "undefined" ? "__styleSet" + Math.floor(Math.random() * 1e8) : Symbol("styleSet");
   const top = typeof globalThis != "undefined" ? globalThis : typeof window != "undefined" ? window : {};

   // :: - Style modules encapsulate a set of CSS rules defined from
   // JavaScript. Their definitions are only available in a given DOM
   // root after it has been _mounted_ there with `StyleModule.mount`.
   //
   // Style modules should be created once and stored somewhere, as
   // opposed to re-creating them every time you need them. The amount of
   // CSS rules generated for a given DOM root is bounded by the amount
   // of style modules that were used. So to avoid leaking rules, don't
   // create these dynamically, but treat them as one-time allocations.
   class StyleModule {
     // :: (Object<Style>, ?{finish: ?(string) → string})
     // Create a style module from the given spec.
     //
     // When `finish` is given, it is called on regular (non-`@`)
     // selectors (after `&` expansion) to compute the final selector.
     constructor(spec, options) {
       this.rules = [];
       let {finish} = options || {};

       function splitSelector(selector) {
         return /^@/.test(selector) ? [selector] : selector.split(/,\s*/)
       }

       function render(selectors, spec, target, isKeyframes) {
         let local = [], isAt = /^@(\w+)\b/.exec(selectors[0]), keyframes = isAt && isAt[1] == "keyframes";
         if (isAt && spec == null) return target.push(selectors[0] + ";")
         for (let prop in spec) {
           let value = spec[prop];
           if (/&/.test(prop)) {
             render(prop.split(/,\s*/).map(part => selectors.map(sel => part.replace(/&/, sel))).reduce((a, b) => a.concat(b)),
                    value, target);
           } else if (value && typeof value == "object") {
             if (!isAt) throw new RangeError("The value of a property (" + prop + ") should be a primitive value.")
             render(splitSelector(prop), value, local, keyframes);
           } else if (value != null) {
             local.push(prop.replace(/_.*/, "").replace(/[A-Z]/g, l => "-" + l.toLowerCase()) + ": " + value + ";");
           }
         }
         if (local.length || keyframes) {
           target.push((finish && !isAt && !isKeyframes ? selectors.map(finish) : selectors).join(", ") +
                       " {" + local.join(" ") + "}");
         }
       }

       for (let prop in spec) render(splitSelector(prop), spec[prop], this.rules);
     }

     // :: () → string
     // Returns a string containing the module's CSS rules.
     getRules() { return this.rules.join("\n") }

     // :: () → string
     // Generate a new unique CSS class name.
     static newName() {
       let id = top[COUNT] || 1;
       top[COUNT] = id + 1;
       return C + id.toString(36)
     }

     // :: (union<Document, ShadowRoot>, union<[StyleModule], StyleModule>)
     //
     // Mount the given set of modules in the given DOM root, which ensures
     // that the CSS rules defined by the module are available in that
     // context.
     //
     // Rules are only added to the document once per root.
     //
     // Rule order will follow the order of the modules, so that rules from
     // modules later in the array take precedence of those from earlier
     // modules. If you call this function multiple times for the same root
     // in a way that changes the order of already mounted modules, the old
     // order will be changed.
     static mount(root, modules) {
       (root[SET] || new StyleSet(root)).mount(Array.isArray(modules) ? modules : [modules]);
     }
   }

   let adoptedSet = null;

   class StyleSet {
     constructor(root) {
       if (!root.head && root.adoptedStyleSheets && typeof CSSStyleSheet != "undefined") {
         if (adoptedSet) {
           root.adoptedStyleSheets = [adoptedSet.sheet].concat(root.adoptedStyleSheets);
           return root[SET] = adoptedSet
         }
         this.sheet = new CSSStyleSheet;
         root.adoptedStyleSheets = [this.sheet].concat(root.adoptedStyleSheets);
         adoptedSet = this;
       } else {
         this.styleTag = (root.ownerDocument || root).createElement("style");
         let target = root.head || root;
         target.insertBefore(this.styleTag, target.firstChild);
       }
       this.modules = [];
       root[SET] = this;
     }

     mount(modules) {
       let sheet = this.sheet;
       let pos = 0 /* Current rule offset */, j = 0; /* Index into this.modules */
       for (let i = 0; i < modules.length; i++) {
         let mod = modules[i], index = this.modules.indexOf(mod);
         if (index < j && index > -1) { // Ordering conflict
           this.modules.splice(index, 1);
           j--;
           index = -1;
         }
         if (index == -1) {
           this.modules.splice(j++, 0, mod);
           if (sheet) for (let k = 0; k < mod.rules.length; k++)
             sheet.insertRule(mod.rules[k], pos++);
         } else {
           while (j < index) pos += this.modules[j++].rules.length;
           pos += mod.rules.length;
           j++;
         }
       }

       if (!sheet) {
         let text = "";
         for (let i = 0; i < this.modules.length; i++)
           text += this.modules[i].getRules() + "\n";
         this.styleTag.textContent = text;
       }
     }
   }

   // Style::Object<union<Style,string>>
   //
   // A style is an object that, in the simple case, maps CSS property
   // names to strings holding their values, as in `{color: "red",
   // fontWeight: "bold"}`. The property names can be given in
   // camel-case—the library will insert a dash before capital letters
   // when converting them to CSS.
   //
   // If you include an underscore in a property name, it and everything
   // after it will be removed from the output, which can be useful when
   // providing a property multiple times, for browser compatibility
   // reasons.
   //
   // A property in a style object can also be a sub-selector, which
   // extends the current context to add a pseudo-selector or a child
   // selector. Such a property should contain a `&` character, which
   // will be replaced by the current selector. For example `{"&:before":
   // {content: '"hi"'}}`. Sub-selectors and regular properties can
   // freely be mixed in a given object. Any property containing a `&` is
   // assumed to be a sub-selector.
   //
   // Finally, a property can specify an @-block to be wrapped around the
   // styles defined inside the object that's the property's value. For
   // example to create a media query you can do `{"@media screen and
   // (min-width: 400px)": {...}}`.

   var base = {
     8: "Backspace",
     9: "Tab",
     10: "Enter",
     12: "NumLock",
     13: "Enter",
     16: "Shift",
     17: "Control",
     18: "Alt",
     20: "CapsLock",
     27: "Escape",
     32: " ",
     33: "PageUp",
     34: "PageDown",
     35: "End",
     36: "Home",
     37: "ArrowLeft",
     38: "ArrowUp",
     39: "ArrowRight",
     40: "ArrowDown",
     44: "PrintScreen",
     45: "Insert",
     46: "Delete",
     59: ";",
     61: "=",
     91: "Meta",
     92: "Meta",
     106: "*",
     107: "+",
     108: ",",
     109: "-",
     110: ".",
     111: "/",
     144: "NumLock",
     145: "ScrollLock",
     160: "Shift",
     161: "Shift",
     162: "Control",
     163: "Control",
     164: "Alt",
     165: "Alt",
     173: "-",
     186: ";",
     187: "=",
     188: ",",
     189: "-",
     190: ".",
     191: "/",
     192: "`",
     219: "[",
     220: "\\",
     221: "]",
     222: "'"
   };

   var shift = {
     48: ")",
     49: "!",
     50: "@",
     51: "#",
     52: "$",
     53: "%",
     54: "^",
     55: "&",
     56: "*",
     57: "(",
     59: ":",
     61: "+",
     173: "_",
     186: ":",
     187: "+",
     188: "<",
     189: "_",
     190: ">",
     191: "?",
     192: "~",
     219: "{",
     220: "|",
     221: "}",
     222: "\""
   };

   var chrome$1 = typeof navigator != "undefined" && /Chrome\/(\d+)/.exec(navigator.userAgent);
   typeof navigator != "undefined" && /Gecko\/\d+/.test(navigator.userAgent);
   var mac = typeof navigator != "undefined" && /Mac/.test(navigator.platform);
   var ie$1 = typeof navigator != "undefined" && /MSIE \d|Trident\/(?:[7-9]|\d{2,})\..*rv:(\d+)/.exec(navigator.userAgent);
   var brokenModifierNames = mac || chrome$1 && +chrome$1[1] < 57;

   // Fill in the digit keys
   for (var i = 0; i < 10; i++) base[48 + i] = base[96 + i] = String(i);

   // The function keys
   for (var i = 1; i <= 24; i++) base[i + 111] = "F" + i;

   // And the alphabetic keys
   for (var i = 65; i <= 90; i++) {
     base[i] = String.fromCharCode(i + 32);
     shift[i] = String.fromCharCode(i);
   }

   // For each code that doesn't have a shift-equivalent, copy the base name
   for (var code in base) if (!shift.hasOwnProperty(code)) shift[code] = base[code];

   function keyName(event) {
     var ignoreKey = brokenModifierNames && (event.ctrlKey || event.altKey || event.metaKey) ||
       ie$1 && event.shiftKey && event.key && event.key.length == 1 ||
       event.key == "Unidentified";
     var name = (!ignoreKey && event.key) ||
       (event.shiftKey ? shift : base)[event.keyCode] ||
       event.key || "Unidentified";
     // Edge sometimes produces wrong names (Issue #3)
     if (name == "Esc") name = "Escape";
     if (name == "Del") name = "Delete";
     // https://developer.microsoft.com/en-us/microsoft-edge/platform/issues/8860571/
     if (name == "Left") name = "ArrowLeft";
     if (name == "Up") name = "ArrowUp";
     if (name == "Right") name = "ArrowRight";
     if (name == "Down") name = "ArrowDown";
     return name
   }

   function getSelection(root) {
       let target;
       // Browsers differ on whether shadow roots have a getSelection
       // method. If it exists, use that, otherwise, call it on the
       // document.
       if (root.nodeType == 11) { // Shadow root
           target = root.getSelection ? root : root.ownerDocument;
       }
       else {
           target = root;
       }
       return target.getSelection();
   }
   function contains(dom, node) {
       return node ? dom == node || dom.contains(node.nodeType != 1 ? node.parentNode : node) : false;
   }
   function deepActiveElement(doc) {
       let elt = doc.activeElement;
       while (elt && elt.shadowRoot)
           elt = elt.shadowRoot.activeElement;
       return elt;
   }
   function hasSelection(dom, selection) {
       if (!selection.anchorNode)
           return false;
       try {
           // Firefox will raise 'permission denied' errors when accessing
           // properties of `sel.anchorNode` when it's in a generated CSS
           // element.
           return contains(dom, selection.anchorNode);
       }
       catch (_) {
           return false;
       }
   }
   function clientRectsFor(dom) {
       if (dom.nodeType == 3)
           return textRange(dom, 0, dom.nodeValue.length).getClientRects();
       else if (dom.nodeType == 1)
           return dom.getClientRects();
       else
           return [];
   }
   // Scans forward and backward through DOM positions equivalent to the
   // given one to see if the two are in the same place (i.e. after a
   // text node vs at the end of that text node)
   function isEquivalentPosition(node, off, targetNode, targetOff) {
       return targetNode ? (scanFor(node, off, targetNode, targetOff, -1) ||
           scanFor(node, off, targetNode, targetOff, 1)) : false;
   }
   function domIndex(node) {
       for (var index = 0;; index++) {
           node = node.previousSibling;
           if (!node)
               return index;
       }
   }
   function scanFor(node, off, targetNode, targetOff, dir) {
       for (;;) {
           if (node == targetNode && off == targetOff)
               return true;
           if (off == (dir < 0 ? 0 : maxOffset(node))) {
               if (node.nodeName == "DIV")
                   return false;
               let parent = node.parentNode;
               if (!parent || parent.nodeType != 1)
                   return false;
               off = domIndex(node) + (dir < 0 ? 0 : 1);
               node = parent;
           }
           else if (node.nodeType == 1) {
               node = node.childNodes[off + (dir < 0 ? -1 : 0)];
               if (node.nodeType == 1 && node.contentEditable == "false")
                   return false;
               off = dir < 0 ? maxOffset(node) : 0;
           }
           else {
               return false;
           }
       }
   }
   function maxOffset(node) {
       return node.nodeType == 3 ? node.nodeValue.length : node.childNodes.length;
   }
   const Rect0 = { left: 0, right: 0, top: 0, bottom: 0 };
   function flattenRect(rect, left) {
       let x = left ? rect.left : rect.right;
       return { left: x, right: x, top: rect.top, bottom: rect.bottom };
   }
   function windowRect(win) {
       return { left: 0, right: win.innerWidth,
           top: 0, bottom: win.innerHeight };
   }
   function scrollRectIntoView(dom, rect, side, x, y, xMargin, yMargin, ltr) {
       let doc = dom.ownerDocument, win = doc.defaultView;
       for (let cur = dom; cur;) {
           if (cur.nodeType == 1) { // Element
               let bounding, top = cur == doc.body;
               if (top) {
                   bounding = windowRect(win);
               }
               else {
                   if (cur.scrollHeight <= cur.clientHeight && cur.scrollWidth <= cur.clientWidth) {
                       cur = cur.parentNode;
                       continue;
                   }
                   let rect = cur.getBoundingClientRect();
                   // Make sure scrollbar width isn't included in the rectangle
                   bounding = { left: rect.left, right: rect.left + cur.clientWidth,
                       top: rect.top, bottom: rect.top + cur.clientHeight };
               }
               let moveX = 0, moveY = 0;
               if (y == "nearest") {
                   if (rect.top < bounding.top) {
                       moveY = -(bounding.top - rect.top + yMargin);
                       if (side > 0 && rect.bottom > bounding.bottom + moveY)
                           moveY = rect.bottom - bounding.bottom + moveY + yMargin;
                   }
                   else if (rect.bottom > bounding.bottom) {
                       moveY = rect.bottom - bounding.bottom + yMargin;
                       if (side < 0 && (rect.top - moveY) < bounding.top)
                           moveY = -(bounding.top + moveY - rect.top + yMargin);
                   }
               }
               else {
                   let rectHeight = rect.bottom - rect.top, boundingHeight = bounding.bottom - bounding.top;
                   let targetTop = y == "center" && rectHeight <= boundingHeight ? rect.top + rectHeight / 2 - boundingHeight / 2 :
                       y == "start" || y == "center" && side < 0 ? rect.top - yMargin :
                           rect.bottom - boundingHeight + yMargin;
                   moveY = targetTop - bounding.top;
               }
               if (x == "nearest") {
                   if (rect.left < bounding.left) {
                       moveX = -(bounding.left - rect.left + xMargin);
                       if (side > 0 && rect.right > bounding.right + moveX)
                           moveX = rect.right - bounding.right + moveX + xMargin;
                   }
                   else if (rect.right > bounding.right) {
                       moveX = rect.right - bounding.right + xMargin;
                       if (side < 0 && rect.left < bounding.left + moveX)
                           moveX = -(bounding.left + moveX - rect.left + xMargin);
                   }
               }
               else {
                   let targetLeft = x == "center" ? rect.left + (rect.right - rect.left) / 2 - (bounding.right - bounding.left) / 2 :
                       (x == "start") == ltr ? rect.left - xMargin :
                           rect.right - (bounding.right - bounding.left) + xMargin;
                   moveX = targetLeft - bounding.left;
               }
               if (moveX || moveY) {
                   if (top) {
                       win.scrollBy(moveX, moveY);
                   }
                   else {
                       if (moveY) {
                           let start = cur.scrollTop;
                           cur.scrollTop += moveY;
                           moveY = cur.scrollTop - start;
                       }
                       if (moveX) {
                           let start = cur.scrollLeft;
                           cur.scrollLeft += moveX;
                           moveX = cur.scrollLeft - start;
                       }
                       rect = { left: rect.left - moveX, top: rect.top - moveY,
                           right: rect.right - moveX, bottom: rect.bottom - moveY };
                   }
               }
               if (top)
                   break;
               cur = cur.assignedSlot || cur.parentNode;
               x = y = "nearest";
           }
           else if (cur.nodeType == 11) { // A shadow root
               cur = cur.host;
           }
           else {
               break;
           }
       }
   }
   class DOMSelectionState {
       constructor() {
           this.anchorNode = null;
           this.anchorOffset = 0;
           this.focusNode = null;
           this.focusOffset = 0;
       }
       eq(domSel) {
           return this.anchorNode == domSel.anchorNode && this.anchorOffset == domSel.anchorOffset &&
               this.focusNode == domSel.focusNode && this.focusOffset == domSel.focusOffset;
       }
       setRange(range) {
           this.set(range.anchorNode, range.anchorOffset, range.focusNode, range.focusOffset);
       }
       set(anchorNode, anchorOffset, focusNode, focusOffset) {
           this.anchorNode = anchorNode;
           this.anchorOffset = anchorOffset;
           this.focusNode = focusNode;
           this.focusOffset = focusOffset;
       }
   }
   let preventScrollSupported = null;
   // Feature-detects support for .focus({preventScroll: true}), and uses
   // a fallback kludge when not supported.
   function focusPreventScroll(dom) {
       if (dom.setActive)
           return dom.setActive(); // in IE
       if (preventScrollSupported)
           return dom.focus(preventScrollSupported);
       let stack = [];
       for (let cur = dom; cur; cur = cur.parentNode) {
           stack.push(cur, cur.scrollTop, cur.scrollLeft);
           if (cur == cur.ownerDocument)
               break;
       }
       dom.focus(preventScrollSupported == null ? {
           get preventScroll() {
               preventScrollSupported = { preventScroll: true };
               return true;
           }
       } : undefined);
       if (!preventScrollSupported) {
           preventScrollSupported = false;
           for (let i = 0; i < stack.length;) {
               let elt = stack[i++], top = stack[i++], left = stack[i++];
               if (elt.scrollTop != top)
                   elt.scrollTop = top;
               if (elt.scrollLeft != left)
                   elt.scrollLeft = left;
           }
       }
   }
   let scratchRange;
   function textRange(node, from, to = from) {
       let range = scratchRange || (scratchRange = document.createRange());
       range.setEnd(node, to);
       range.setStart(node, from);
       return range;
   }
   function dispatchKey(elt, name, code) {
       let options = { key: name, code: name, keyCode: code, which: code, cancelable: true };
       let down = new KeyboardEvent("keydown", options);
       down.synthetic = true;
       elt.dispatchEvent(down);
       let up = new KeyboardEvent("keyup", options);
       up.synthetic = true;
       elt.dispatchEvent(up);
       return down.defaultPrevented || up.defaultPrevented;
   }
   function getRoot(node) {
       while (node) {
           if (node && (node.nodeType == 9 || node.nodeType == 11 && node.host))
               return node;
           node = node.assignedSlot || node.parentNode;
       }
       return null;
   }
   function clearAttributes(node) {
       while (node.attributes.length)
           node.removeAttributeNode(node.attributes[0]);
   }
   function atElementStart(doc, selection) {
       let node = selection.focusNode, offset = selection.focusOffset;
       if (!node || selection.anchorNode != node || selection.anchorOffset != offset)
           return false;
       for (;;) {
           if (offset) {
               if (node.nodeType != 1)
                   return false;
               let prev = node.childNodes[offset - 1];
               if (prev.contentEditable == "false")
                   offset--;
               else {
                   node = prev;
                   offset = maxOffset(node);
               }
           }
           else if (node == doc) {
               return true;
           }
           else {
               offset = domIndex(node);
               node = node.parentNode;
           }
       }
   }

   class DOMPos {
       constructor(node, offset, precise = true) {
           this.node = node;
           this.offset = offset;
           this.precise = precise;
       }
       static before(dom, precise) { return new DOMPos(dom.parentNode, domIndex(dom), precise); }
       static after(dom, precise) { return new DOMPos(dom.parentNode, domIndex(dom) + 1, precise); }
   }
   const noChildren = [];
   class ContentView {
       constructor() {
           this.parent = null;
           this.dom = null;
           this.dirty = 2 /* Dirty.Node */;
       }
       get editorView() {
           if (!this.parent)
               throw new Error("Accessing view in orphan content view");
           return this.parent.editorView;
       }
       get overrideDOMText() { return null; }
       get posAtStart() {
           return this.parent ? this.parent.posBefore(this) : 0;
       }
       get posAtEnd() {
           return this.posAtStart + this.length;
       }
       posBefore(view) {
           let pos = this.posAtStart;
           for (let child of this.children) {
               if (child == view)
                   return pos;
               pos += child.length + child.breakAfter;
           }
           throw new RangeError("Invalid child in posBefore");
       }
       posAfter(view) {
           return this.posBefore(view) + view.length;
       }
       // Will return a rectangle directly before (when side < 0), after
       // (side > 0) or directly on (when the browser supports it) the
       // given position.
       coordsAt(_pos, _side) { return null; }
       sync(track) {
           if (this.dirty & 2 /* Dirty.Node */) {
               let parent = this.dom;
               let prev = null, next;
               for (let child of this.children) {
                   if (child.dirty) {
                       if (!child.dom && (next = prev ? prev.nextSibling : parent.firstChild)) {
                           let contentView = ContentView.get(next);
                           if (!contentView || !contentView.parent && contentView.constructor == child.constructor)
                               child.reuseDOM(next);
                       }
                       child.sync(track);
                       child.dirty = 0 /* Dirty.Not */;
                   }
                   next = prev ? prev.nextSibling : parent.firstChild;
                   if (track && !track.written && track.node == parent && next != child.dom)
                       track.written = true;
                   if (child.dom.parentNode == parent) {
                       while (next && next != child.dom)
                           next = rm$1(next);
                   }
                   else {
                       parent.insertBefore(child.dom, next);
                   }
                   prev = child.dom;
               }
               next = prev ? prev.nextSibling : parent.firstChild;
               if (next && track && track.node == parent)
                   track.written = true;
               while (next)
                   next = rm$1(next);
           }
           else if (this.dirty & 1 /* Dirty.Child */) {
               for (let child of this.children)
                   if (child.dirty) {
                       child.sync(track);
                       child.dirty = 0 /* Dirty.Not */;
                   }
           }
       }
       reuseDOM(_dom) { }
       localPosFromDOM(node, offset) {
           let after;
           if (node == this.dom) {
               after = this.dom.childNodes[offset];
           }
           else {
               let bias = maxOffset(node) == 0 ? 0 : offset == 0 ? -1 : 1;
               for (;;) {
                   let parent = node.parentNode;
                   if (parent == this.dom)
                       break;
                   if (bias == 0 && parent.firstChild != parent.lastChild) {
                       if (node == parent.firstChild)
                           bias = -1;
                       else
                           bias = 1;
                   }
                   node = parent;
               }
               if (bias < 0)
                   after = node;
               else
                   after = node.nextSibling;
           }
           if (after == this.dom.firstChild)
               return 0;
           while (after && !ContentView.get(after))
               after = after.nextSibling;
           if (!after)
               return this.length;
           for (let i = 0, pos = 0;; i++) {
               let child = this.children[i];
               if (child.dom == after)
                   return pos;
               pos += child.length + child.breakAfter;
           }
       }
       domBoundsAround(from, to, offset = 0) {
           let fromI = -1, fromStart = -1, toI = -1, toEnd = -1;
           for (let i = 0, pos = offset, prevEnd = offset; i < this.children.length; i++) {
               let child = this.children[i], end = pos + child.length;
               if (pos < from && end > to)
                   return child.domBoundsAround(from, to, pos);
               if (end >= from && fromI == -1) {
                   fromI = i;
                   fromStart = pos;
               }
               if (pos > to && child.dom.parentNode == this.dom) {
                   toI = i;
                   toEnd = prevEnd;
                   break;
               }
               prevEnd = end;
               pos = end + child.breakAfter;
           }
           return { from: fromStart, to: toEnd < 0 ? offset + this.length : toEnd,
               startDOM: (fromI ? this.children[fromI - 1].dom.nextSibling : null) || this.dom.firstChild,
               endDOM: toI < this.children.length && toI >= 0 ? this.children[toI].dom : null };
       }
       markDirty(andParent = false) {
           this.dirty |= 2 /* Dirty.Node */;
           this.markParentsDirty(andParent);
       }
       markParentsDirty(childList) {
           for (let parent = this.parent; parent; parent = parent.parent) {
               if (childList)
                   parent.dirty |= 2 /* Dirty.Node */;
               if (parent.dirty & 1 /* Dirty.Child */)
                   return;
               parent.dirty |= 1 /* Dirty.Child */;
               childList = false;
           }
       }
       setParent(parent) {
           if (this.parent != parent) {
               this.parent = parent;
               if (this.dirty)
                   this.markParentsDirty(true);
           }
       }
       setDOM(dom) {
           if (this.dom)
               this.dom.cmView = null;
           this.dom = dom;
           dom.cmView = this;
       }
       get rootView() {
           for (let v = this;;) {
               let parent = v.parent;
               if (!parent)
                   return v;
               v = parent;
           }
       }
       replaceChildren(from, to, children = noChildren) {
           this.markDirty();
           for (let i = from; i < to; i++) {
               let child = this.children[i];
               if (child.parent == this)
                   child.destroy();
           }
           this.children.splice(from, to - from, ...children);
           for (let i = 0; i < children.length; i++)
               children[i].setParent(this);
       }
       ignoreMutation(_rec) { return false; }
       ignoreEvent(_event) { return false; }
       childCursor(pos = this.length) {
           return new ChildCursor(this.children, pos, this.children.length);
       }
       childPos(pos, bias = 1) {
           return this.childCursor().findPos(pos, bias);
       }
       toString() {
           let name = this.constructor.name.replace("View", "");
           return name + (this.children.length ? "(" + this.children.join() + ")" :
               this.length ? "[" + (name == "Text" ? this.text : this.length) + "]" : "") +
               (this.breakAfter ? "#" : "");
       }
       static get(node) { return node.cmView; }
       get isEditable() { return true; }
       merge(from, to, source, hasStart, openStart, openEnd) {
           return false;
       }
       become(other) { return false; }
       // When this is a zero-length view with a side, this should return a
       // number <= 0 to indicate it is before its position, or a
       // number > 0 when after its position.
       getSide() { return 0; }
       destroy() {
           this.parent = null;
       }
   }
   ContentView.prototype.breakAfter = 0;
   // Remove a DOM node and return its next sibling.
   function rm$1(dom) {
       let next = dom.nextSibling;
       dom.parentNode.removeChild(dom);
       return next;
   }
   class ChildCursor {
       constructor(children, pos, i) {
           this.children = children;
           this.pos = pos;
           this.i = i;
           this.off = 0;
       }
       findPos(pos, bias = 1) {
           for (;;) {
               if (pos > this.pos || pos == this.pos &&
                   (bias > 0 || this.i == 0 || this.children[this.i - 1].breakAfter)) {
                   this.off = pos - this.pos;
                   return this;
               }
               let next = this.children[--this.i];
               this.pos -= next.length + next.breakAfter;
           }
       }
   }
   function replaceRange(parent, fromI, fromOff, toI, toOff, insert, breakAtStart, openStart, openEnd) {
       let { children } = parent;
       let before = children.length ? children[fromI] : null;
       let last = insert.length ? insert[insert.length - 1] : null;
       let breakAtEnd = last ? last.breakAfter : breakAtStart;
       // Change within a single child
       if (fromI == toI && before && !breakAtStart && !breakAtEnd && insert.length < 2 &&
           before.merge(fromOff, toOff, insert.length ? last : null, fromOff == 0, openStart, openEnd))
           return;
       if (toI < children.length) {
           let after = children[toI];
           // Make sure the end of the child after the update is preserved in `after`
           if (after && toOff < after.length) {
               // If we're splitting a child, separate part of it to avoid that
               // being mangled when updating the child before the update.
               if (fromI == toI) {
                   after = after.split(toOff);
                   toOff = 0;
               }
               // If the element after the replacement should be merged with
               // the last replacing element, update `content`
               if (!breakAtEnd && last && after.merge(0, toOff, last, true, 0, openEnd)) {
                   insert[insert.length - 1] = after;
               }
               else {
                   // Remove the start of the after element, if necessary, and
                   // add it to `content`.
                   if (toOff)
                       after.merge(0, toOff, null, false, 0, openEnd);
                   insert.push(after);
               }
           }
           else if (after === null || after === void 0 ? void 0 : after.breakAfter) {
               // The element at `toI` is entirely covered by this range.
               // Preserve its line break, if any.
               if (last)
                   last.breakAfter = 1;
               else
                   breakAtStart = 1;
           }
           // Since we've handled the next element from the current elements
           // now, make sure `toI` points after that.
           toI++;
       }
       if (before) {
           before.breakAfter = breakAtStart;
           if (fromOff > 0) {
               if (!breakAtStart && insert.length && before.merge(fromOff, before.length, insert[0], false, openStart, 0)) {
                   before.breakAfter = insert.shift().breakAfter;
               }
               else if (fromOff < before.length || before.children.length && before.children[before.children.length - 1].length == 0) {
                   before.merge(fromOff, before.length, null, false, openStart, 0);
               }
               fromI++;
           }
       }
       // Try to merge widgets on the boundaries of the replacement
       while (fromI < toI && insert.length) {
           if (children[toI - 1].become(insert[insert.length - 1])) {
               toI--;
               insert.pop();
               openEnd = insert.length ? 0 : openStart;
           }
           else if (children[fromI].become(insert[0])) {
               fromI++;
               insert.shift();
               openStart = insert.length ? 0 : openEnd;
           }
           else {
               break;
           }
       }
       if (!insert.length && fromI && toI < children.length && !children[fromI - 1].breakAfter &&
           children[toI].merge(0, 0, children[fromI - 1], false, openStart, openEnd))
           fromI--;
       if (fromI < toI || insert.length)
           parent.replaceChildren(fromI, toI, insert);
   }
   function mergeChildrenInto(parent, from, to, insert, openStart, openEnd) {
       let cur = parent.childCursor();
       let { i: toI, off: toOff } = cur.findPos(to, 1);
       let { i: fromI, off: fromOff } = cur.findPos(from, -1);
       let dLen = from - to;
       for (let view of insert)
           dLen += view.length;
       parent.length += dLen;
       replaceRange(parent, fromI, fromOff, toI, toOff, insert, 0, openStart, openEnd);
   }

   let nav = typeof navigator != "undefined" ? navigator : { userAgent: "", vendor: "", platform: "" };
   let doc = typeof document != "undefined" ? document : { documentElement: { style: {} } };
   const ie_edge = /*@__PURE__*//Edge\/(\d+)/.exec(nav.userAgent);
   const ie_upto10 = /*@__PURE__*//MSIE \d/.test(nav.userAgent);
   const ie_11up = /*@__PURE__*//Trident\/(?:[7-9]|\d{2,})\..*rv:(\d+)/.exec(nav.userAgent);
   const ie = !!(ie_upto10 || ie_11up || ie_edge);
   const gecko = !ie && /*@__PURE__*//gecko\/(\d+)/i.test(nav.userAgent);
   const chrome = !ie && /*@__PURE__*//Chrome\/(\d+)/.exec(nav.userAgent);
   const webkit = "webkitFontSmoothing" in doc.documentElement.style;
   const safari = !ie && /*@__PURE__*//Apple Computer/.test(nav.vendor);
   const ios = safari && (/*@__PURE__*//Mobile\/\w+/.test(nav.userAgent) || nav.maxTouchPoints > 2);
   var browser = {
       mac: ios || /*@__PURE__*//Mac/.test(nav.platform),
       windows: /*@__PURE__*//Win/.test(nav.platform),
       linux: /*@__PURE__*//Linux|X11/.test(nav.platform),
       ie,
       ie_version: ie_upto10 ? doc.documentMode || 6 : ie_11up ? +ie_11up[1] : ie_edge ? +ie_edge[1] : 0,
       gecko,
       gecko_version: gecko ? +(/*@__PURE__*//Firefox\/(\d+)/.exec(nav.userAgent) || [0, 0])[1] : 0,
       chrome: !!chrome,
       chrome_version: chrome ? +chrome[1] : 0,
       ios,
       android: /*@__PURE__*//Android\b/.test(nav.userAgent),
       webkit,
       safari,
       webkit_version: webkit ? +(/*@__PURE__*//\bAppleWebKit\/(\d+)/.exec(navigator.userAgent) || [0, 0])[1] : 0,
       tabSize: doc.documentElement.style.tabSize != null ? "tab-size" : "-moz-tab-size"
   };

   const MaxJoinLen = 256;
   class TextView extends ContentView {
       constructor(text) {
           super();
           this.text = text;
       }
       get length() { return this.text.length; }
       createDOM(textDOM) {
           this.setDOM(textDOM || document.createTextNode(this.text));
       }
       sync(track) {
           if (!this.dom)
               this.createDOM();
           if (this.dom.nodeValue != this.text) {
               if (track && track.node == this.dom)
                   track.written = true;
               this.dom.nodeValue = this.text;
           }
       }
       reuseDOM(dom) {
           if (dom.nodeType == 3)
               this.createDOM(dom);
       }
       merge(from, to, source) {
           if (source && (!(source instanceof TextView) || this.length - (to - from) + source.length > MaxJoinLen))
               return false;
           this.text = this.text.slice(0, from) + (source ? source.text : "") + this.text.slice(to);
           this.markDirty();
           return true;
       }
       split(from) {
           let result = new TextView(this.text.slice(from));
           this.text = this.text.slice(0, from);
           this.markDirty();
           return result;
       }
       localPosFromDOM(node, offset) {
           return node == this.dom ? offset : offset ? this.text.length : 0;
       }
       domAtPos(pos) { return new DOMPos(this.dom, pos); }
       domBoundsAround(_from, _to, offset) {
           return { from: offset, to: offset + this.length, startDOM: this.dom, endDOM: this.dom.nextSibling };
       }
       coordsAt(pos, side) {
           return textCoords(this.dom, pos, side);
       }
   }
   class MarkView extends ContentView {
       constructor(mark, children = [], length = 0) {
           super();
           this.mark = mark;
           this.children = children;
           this.length = length;
           for (let ch of children)
               ch.setParent(this);
       }
       setAttrs(dom) {
           clearAttributes(dom);
           if (this.mark.class)
               dom.className = this.mark.class;
           if (this.mark.attrs)
               for (let name in this.mark.attrs)
                   dom.setAttribute(name, this.mark.attrs[name]);
           return dom;
       }
       reuseDOM(node) {
           if (node.nodeName == this.mark.tagName.toUpperCase()) {
               this.setDOM(node);
               this.dirty |= 4 /* Dirty.Attrs */ | 2 /* Dirty.Node */;
           }
       }
       sync(track) {
           if (!this.dom)
               this.setDOM(this.setAttrs(document.createElement(this.mark.tagName)));
           else if (this.dirty & 4 /* Dirty.Attrs */)
               this.setAttrs(this.dom);
           super.sync(track);
       }
       merge(from, to, source, _hasStart, openStart, openEnd) {
           if (source && (!(source instanceof MarkView && source.mark.eq(this.mark)) ||
               (from && openStart <= 0) || (to < this.length && openEnd <= 0)))
               return false;
           mergeChildrenInto(this, from, to, source ? source.children : [], openStart - 1, openEnd - 1);
           this.markDirty();
           return true;
       }
       split(from) {
           let result = [], off = 0, detachFrom = -1, i = 0;
           for (let elt of this.children) {
               let end = off + elt.length;
               if (end > from)
                   result.push(off < from ? elt.split(from - off) : elt);
               if (detachFrom < 0 && off >= from)
                   detachFrom = i;
               off = end;
               i++;
           }
           let length = this.length - from;
           this.length = from;
           if (detachFrom > -1) {
               this.children.length = detachFrom;
               this.markDirty();
           }
           return new MarkView(this.mark, result, length);
       }
       domAtPos(pos) {
           return inlineDOMAtPos(this, pos);
       }
       coordsAt(pos, side) {
           return coordsInChildren(this, pos, side);
       }
   }
   function textCoords(text, pos, side) {
       let length = text.nodeValue.length;
       if (pos > length)
           pos = length;
       let from = pos, to = pos, flatten = 0;
       if (pos == 0 && side < 0 || pos == length && side >= 0) {
           if (!(browser.chrome || browser.gecko)) { // These browsers reliably return valid rectangles for empty ranges
               if (pos) {
                   from--;
                   flatten = 1;
               } // FIXME this is wrong in RTL text
               else if (to < length) {
                   to++;
                   flatten = -1;
               }
           }
       }
       else {
           if (side < 0)
               from--;
           else if (to < length)
               to++;
       }
       let rects = textRange(text, from, to).getClientRects();
       if (!rects.length)
           return Rect0;
       let rect = rects[(flatten ? flatten < 0 : side >= 0) ? 0 : rects.length - 1];
       if (browser.safari && !flatten && rect.width == 0)
           rect = Array.prototype.find.call(rects, r => r.width) || rect;
       return flatten ? flattenRect(rect, flatten < 0) : rect || null;
   }
   // Also used for collapsed ranges that don't have a placeholder widget!
   class WidgetView extends ContentView {
       constructor(widget, length, side) {
           super();
           this.widget = widget;
           this.length = length;
           this.side = side;
           this.prevWidget = null;
       }
       static create(widget, length, side) {
           return new (widget.customView || WidgetView)(widget, length, side);
       }
       split(from) {
           let result = WidgetView.create(this.widget, this.length - from, this.side);
           this.length -= from;
           return result;
       }
       sync() {
           if (!this.dom || !this.widget.updateDOM(this.dom)) {
               if (this.dom && this.prevWidget)
                   this.prevWidget.destroy(this.dom);
               this.prevWidget = null;
               this.setDOM(this.widget.toDOM(this.editorView));
               this.dom.contentEditable = "false";
           }
       }
       getSide() { return this.side; }
       merge(from, to, source, hasStart, openStart, openEnd) {
           if (source && (!(source instanceof WidgetView) || !this.widget.compare(source.widget) ||
               from > 0 && openStart <= 0 || to < this.length && openEnd <= 0))
               return false;
           this.length = from + (source ? source.length : 0) + (this.length - to);
           return true;
       }
       become(other) {
           if (other.length == this.length && other instanceof WidgetView && other.side == this.side) {
               if (this.widget.constructor == other.widget.constructor) {
                   if (!this.widget.eq(other.widget))
                       this.markDirty(true);
                   if (this.dom && !this.prevWidget)
                       this.prevWidget = this.widget;
                   this.widget = other.widget;
                   return true;
               }
           }
           return false;
       }
       ignoreMutation() { return true; }
       ignoreEvent(event) { return this.widget.ignoreEvent(event); }
       get overrideDOMText() {
           if (this.length == 0)
               return Text.empty;
           let top = this;
           while (top.parent)
               top = top.parent;
           let view = top.editorView, text = view && view.state.doc, start = this.posAtStart;
           return text ? text.slice(start, start + this.length) : Text.empty;
       }
       domAtPos(pos) {
           return pos == 0 ? DOMPos.before(this.dom) : DOMPos.after(this.dom, pos == this.length);
       }
       domBoundsAround() { return null; }
       coordsAt(pos, side) {
           let rects = this.dom.getClientRects(), rect = null;
           if (!rects.length)
               return Rect0;
           for (let i = pos > 0 ? rects.length - 1 : 0;; i += (pos > 0 ? -1 : 1)) {
               rect = rects[i];
               if (pos > 0 ? i == 0 : i == rects.length - 1 || rect.top < rect.bottom)
                   break;
           }
           return (pos == 0 && side > 0 || pos == this.length && side <= 0) ? rect : flattenRect(rect, pos == 0);
       }
       get isEditable() { return false; }
       destroy() {
           super.destroy();
           if (this.dom)
               this.widget.destroy(this.dom);
       }
   }
   class CompositionView extends WidgetView {
       domAtPos(pos) {
           let { topView, text } = this.widget;
           if (!topView)
               return new DOMPos(text, Math.min(pos, text.nodeValue.length));
           return scanCompositionTree(pos, 0, topView, text, (v, p) => v.domAtPos(p), p => new DOMPos(text, Math.min(p, text.nodeValue.length)));
       }
       sync() { this.setDOM(this.widget.toDOM()); }
       localPosFromDOM(node, offset) {
           let { topView, text } = this.widget;
           if (!topView)
               return Math.min(offset, this.length);
           return posFromDOMInCompositionTree(node, offset, topView, text);
       }
       ignoreMutation() { return false; }
       get overrideDOMText() { return null; }
       coordsAt(pos, side) {
           let { topView, text } = this.widget;
           if (!topView)
               return textCoords(text, pos, side);
           return scanCompositionTree(pos, side, topView, text, (v, pos, side) => v.coordsAt(pos, side), (pos, side) => textCoords(text, pos, side));
       }
       destroy() {
           var _a;
           super.destroy();
           (_a = this.widget.topView) === null || _a === void 0 ? void 0 : _a.destroy();
       }
       get isEditable() { return true; }
   }
   // Uses the old structure of a chunk of content view frozen for
   // composition to try and find a reasonable DOM location for the given
   // offset.
   function scanCompositionTree(pos, side, view, text, enterView, fromText) {
       if (view instanceof MarkView) {
           for (let child = view.dom.firstChild; child; child = child.nextSibling) {
               let desc = ContentView.get(child);
               if (!desc)
                   return fromText(pos, side);
               let hasComp = contains(child, text);
               let len = desc.length + (hasComp ? text.nodeValue.length : 0);
               if (pos < len || pos == len && desc.getSide() <= 0)
                   return hasComp ? scanCompositionTree(pos, side, desc, text, enterView, fromText) : enterView(desc, pos, side);
               pos -= len;
           }
           return enterView(view, view.length, -1);
       }
       else if (view.dom == text) {
           return fromText(pos, side);
       }
       else {
           return enterView(view, pos, side);
       }
   }
   function posFromDOMInCompositionTree(node, offset, view, text) {
       if (view instanceof MarkView) {
           for (let child of view.children) {
               let pos = 0, hasComp = contains(child.dom, text);
               if (contains(child.dom, node))
                   return pos + (hasComp ? posFromDOMInCompositionTree(node, offset, child, text) : child.localPosFromDOM(node, offset));
               pos += hasComp ? text.nodeValue.length : child.length;
           }
       }
       else if (view.dom == text) {
           return Math.min(offset, text.nodeValue.length);
       }
       return view.localPosFromDOM(node, offset);
   }
   // These are drawn around uneditable widgets to avoid a number of
   // browser bugs that show up when the cursor is directly next to
   // uneditable inline content.
   class WidgetBufferView extends ContentView {
       constructor(side) {
           super();
           this.side = side;
       }
       get length() { return 0; }
       merge() { return false; }
       become(other) {
           return other instanceof WidgetBufferView && other.side == this.side;
       }
       split() { return new WidgetBufferView(this.side); }
       sync() {
           if (!this.dom) {
               let dom = document.createElement("img");
               dom.className = "cm-widgetBuffer";
               dom.setAttribute("aria-hidden", "true");
               this.setDOM(dom);
           }
       }
       getSide() { return this.side; }
       domAtPos(pos) { return DOMPos.before(this.dom); }
       localPosFromDOM() { return 0; }
       domBoundsAround() { return null; }
       coordsAt(pos) {
           let imgRect = this.dom.getBoundingClientRect();
           // Since the <img> height doesn't correspond to text height, try
           // to borrow the height from some sibling node.
           let siblingRect = inlineSiblingRect(this, this.side > 0 ? -1 : 1);
           return siblingRect && siblingRect.top < imgRect.bottom && siblingRect.bottom > imgRect.top
               ? { left: imgRect.left, right: imgRect.right, top: siblingRect.top, bottom: siblingRect.bottom } : imgRect;
       }
       get overrideDOMText() {
           return Text.empty;
       }
   }
   TextView.prototype.children = WidgetView.prototype.children = WidgetBufferView.prototype.children = noChildren;
   function inlineSiblingRect(view, side) {
       let parent = view.parent, index = parent ? parent.children.indexOf(view) : -1;
       while (parent && index >= 0) {
           if (side < 0 ? index > 0 : index < parent.children.length) {
               let next = parent.children[index + side];
               if (next instanceof TextView) {
                   let nextRect = next.coordsAt(side < 0 ? next.length : 0, side);
                   if (nextRect)
                       return nextRect;
               }
               index += side;
           }
           else if (parent instanceof MarkView && parent.parent) {
               index = parent.parent.children.indexOf(parent) + (side < 0 ? 0 : 1);
               parent = parent.parent;
           }
           else {
               let last = parent.dom.lastChild;
               if (last && last.nodeName == "BR")
                   return last.getClientRects()[0];
               break;
           }
       }
       return undefined;
   }
   function inlineDOMAtPos(parent, pos) {
       let dom = parent.dom, { children } = parent, i = 0;
       for (let off = 0; i < children.length; i++) {
           let child = children[i], end = off + child.length;
           if (end == off && child.getSide() <= 0)
               continue;
           if (pos > off && pos < end && child.dom.parentNode == dom)
               return child.domAtPos(pos - off);
           if (pos <= off)
               break;
           off = end;
       }
       //  if (i) return DOMPos.after(children[i - 1].dom!)
       for (let j = i; j > 0; j--) {
           let prev = children[j - 1];
           if (prev.dom.parentNode == dom)
               return prev.domAtPos(prev.length);
       }
       for (let j = i; j < children.length; j++) {
           let next = children[j];
           if (next.dom.parentNode == dom)
               return next.domAtPos(0);
       }
       return new DOMPos(dom, 0);
   }
   // Assumes `view`, if a mark view, has precisely 1 child.
   function joinInlineInto(parent, view, open) {
       let last, { children } = parent;
       if (open > 0 && view instanceof MarkView && children.length &&
           (last = children[children.length - 1]) instanceof MarkView && last.mark.eq(view.mark)) {
           joinInlineInto(last, view.children[0], open - 1);
       }
       else {
           children.push(view);
           view.setParent(parent);
       }
       parent.length += view.length;
   }
   function coordsInChildren(view, pos, side) {
       for (let off = 0, i = 0; i < view.children.length; i++) {
           let child = view.children[i], end = off + child.length, next;
           if ((side <= 0 || end == view.length || child.getSide() > 0 ? end >= pos : end > pos) &&
               (pos < end || i + 1 == view.children.length || (next = view.children[i + 1]).length || next.getSide() > 0)) {
               let flatten = 0;
               if (end == off) {
                   if (child.getSide() <= 0)
                       continue;
                   flatten = side = -child.getSide();
               }
               let rect = child.coordsAt(Math.max(0, pos - off), side);
               return flatten && rect ? flattenRect(rect, side < 0) : rect;
           }
           off = end;
       }
       let last = view.dom.lastChild;
       if (!last)
           return view.dom.getBoundingClientRect();
       let rects = clientRectsFor(last);
       return rects[rects.length - 1] || null;
   }

   function combineAttrs(source, target) {
       for (let name in source) {
           if (name == "class" && target.class)
               target.class += " " + source.class;
           else if (name == "style" && target.style)
               target.style += ";" + source.style;
           else
               target[name] = source[name];
       }
       return target;
   }
   function attrsEq(a, b) {
       if (a == b)
           return true;
       if (!a || !b)
           return false;
       let keysA = Object.keys(a), keysB = Object.keys(b);
       if (keysA.length != keysB.length)
           return false;
       for (let key of keysA) {
           if (keysB.indexOf(key) == -1 || a[key] !== b[key])
               return false;
       }
       return true;
   }
   function updateAttrs(dom, prev, attrs) {
       let changed = null;
       if (prev)
           for (let name in prev)
               if (!(attrs && name in attrs))
                   dom.removeAttribute(changed = name);
       if (attrs)
           for (let name in attrs)
               if (!(prev && prev[name] == attrs[name]))
                   dom.setAttribute(changed = name, attrs[name]);
       return !!changed;
   }

   /**
   Widgets added to the content are described by subclasses of this
   class. Using a description object like that makes it possible to
   delay creating of the DOM structure for a widget until it is
   needed, and to avoid redrawing widgets even if the decorations
   that define them are recreated.
   */
   class WidgetType {
       /**
       Compare this instance to another instance of the same type.
       (TypeScript can't express this, but only instances of the same
       specific class will be passed to this method.) This is used to
       avoid redrawing widgets when they are replaced by a new
       decoration of the same type. The default implementation just
       returns `false`, which will cause new instances of the widget to
       always be redrawn.
       */
       eq(widget) { return false; }
       /**
       Update a DOM element created by a widget of the same type (but
       different, non-`eq` content) to reflect this widget. May return
       true to indicate that it could update, false to indicate it
       couldn't (in which case the widget will be redrawn). The default
       implementation just returns false.
       */
       updateDOM(dom) { return false; }
       /**
       @internal
       */
       compare(other) {
           return this == other || this.constructor == other.constructor && this.eq(other);
       }
       /**
       The estimated height this widget will have, to be used when
       estimating the height of content that hasn't been drawn. May
       return -1 to indicate you don't know. The default implementation
       returns -1.
       */
       get estimatedHeight() { return -1; }
       /**
       Can be used to configure which kinds of events inside the widget
       should be ignored by the editor. The default is to ignore all
       events.
       */
       ignoreEvent(event) { return true; }
       /**
       @internal
       */
       get customView() { return null; }
       /**
       This is called when the an instance of the widget is removed
       from the editor view.
       */
       destroy(dom) { }
   }
   /**
   The different types of blocks that can occur in an editor view.
   */
   var BlockType = /*@__PURE__*/(function (BlockType) {
       /**
       A line of text.
       */
       BlockType[BlockType["Text"] = 0] = "Text";
       /**
       A block widget associated with the position after it.
       */
       BlockType[BlockType["WidgetBefore"] = 1] = "WidgetBefore";
       /**
       A block widget associated with the position before it.
       */
       BlockType[BlockType["WidgetAfter"] = 2] = "WidgetAfter";
       /**
       A block widget [replacing](https://codemirror.net/6/docs/ref/#view.Decoration^replace) a range of content.
       */
       BlockType[BlockType["WidgetRange"] = 3] = "WidgetRange";
   return BlockType})(BlockType || (BlockType = {}));
   /**
   A decoration provides information on how to draw or style a piece
   of content. You'll usually use it wrapped in a
   [`Range`](https://codemirror.net/6/docs/ref/#state.Range), which adds a start and end position.
   @nonabstract
   */
   class Decoration extends RangeValue {
       constructor(
       /**
       @internal
       */
       startSide, 
       /**
       @internal
       */
       endSide, 
       /**
       @internal
       */
       widget, 
       /**
       The config object used to create this decoration. You can
       include additional properties in there to store metadata about
       your decoration.
       */
       spec) {
           super();
           this.startSide = startSide;
           this.endSide = endSide;
           this.widget = widget;
           this.spec = spec;
       }
       /**
       @internal
       */
       get heightRelevant() { return false; }
       /**
       Create a mark decoration, which influences the styling of the
       content in its range. Nested mark decorations will cause nested
       DOM elements to be created. Nesting order is determined by
       precedence of the [facet](https://codemirror.net/6/docs/ref/#view.EditorView^decorations), with
       the higher-precedence decorations creating the inner DOM nodes.
       Such elements are split on line boundaries and on the boundaries
       of lower-precedence decorations.
       */
       static mark(spec) {
           return new MarkDecoration(spec);
       }
       /**
       Create a widget decoration, which displays a DOM element at the
       given position.
       */
       static widget(spec) {
           let side = spec.side || 0, block = !!spec.block;
           side += block ? (side > 0 ? 300000000 /* Side.BlockAfter */ : -400000000 /* Side.BlockBefore */) : (side > 0 ? 100000000 /* Side.InlineAfter */ : -100000000 /* Side.InlineBefore */);
           return new PointDecoration(spec, side, side, block, spec.widget || null, false);
       }
       /**
       Create a replace decoration which replaces the given range with
       a widget, or simply hides it.
       */
       static replace(spec) {
           let block = !!spec.block, startSide, endSide;
           if (spec.isBlockGap) {
               startSide = -500000000 /* Side.GapStart */;
               endSide = 400000000 /* Side.GapEnd */;
           }
           else {
               let { start, end } = getInclusive(spec, block);
               startSide = (start ? (block ? -300000000 /* Side.BlockIncStart */ : -1 /* Side.InlineIncStart */) : 500000000 /* Side.NonIncStart */) - 1;
               endSide = (end ? (block ? 200000000 /* Side.BlockIncEnd */ : 1 /* Side.InlineIncEnd */) : -600000000 /* Side.NonIncEnd */) + 1;
           }
           return new PointDecoration(spec, startSide, endSide, block, spec.widget || null, true);
       }
       /**
       Create a line decoration, which can add DOM attributes to the
       line starting at the given position.
       */
       static line(spec) {
           return new LineDecoration(spec);
       }
       /**
       Build a [`DecorationSet`](https://codemirror.net/6/docs/ref/#view.DecorationSet) from the given
       decorated range or ranges. If the ranges aren't already sorted,
       pass `true` for `sort` to make the library sort them for you.
       */
       static set(of, sort = false) {
           return RangeSet.of(of, sort);
       }
       /**
       @internal
       */
       hasHeight() { return this.widget ? this.widget.estimatedHeight > -1 : false; }
   }
   /**
   The empty set of decorations.
   */
   Decoration.none = RangeSet.empty;
   class MarkDecoration extends Decoration {
       constructor(spec) {
           let { start, end } = getInclusive(spec);
           super(start ? -1 /* Side.InlineIncStart */ : 500000000 /* Side.NonIncStart */, end ? 1 /* Side.InlineIncEnd */ : -600000000 /* Side.NonIncEnd */, null, spec);
           this.tagName = spec.tagName || "span";
           this.class = spec.class || "";
           this.attrs = spec.attributes || null;
       }
       eq(other) {
           return this == other ||
               other instanceof MarkDecoration &&
                   this.tagName == other.tagName &&
                   this.class == other.class &&
                   attrsEq(this.attrs, other.attrs);
       }
       range(from, to = from) {
           if (from >= to)
               throw new RangeError("Mark decorations may not be empty");
           return super.range(from, to);
       }
   }
   MarkDecoration.prototype.point = false;
   class LineDecoration extends Decoration {
       constructor(spec) {
           super(-200000000 /* Side.Line */, -200000000 /* Side.Line */, null, spec);
       }
       eq(other) {
           return other instanceof LineDecoration && attrsEq(this.spec.attributes, other.spec.attributes);
       }
       range(from, to = from) {
           if (to != from)
               throw new RangeError("Line decoration ranges must be zero-length");
           return super.range(from, to);
       }
   }
   LineDecoration.prototype.mapMode = MapMode.TrackBefore;
   LineDecoration.prototype.point = true;
   class PointDecoration extends Decoration {
       constructor(spec, startSide, endSide, block, widget, isReplace) {
           super(startSide, endSide, widget, spec);
           this.block = block;
           this.isReplace = isReplace;
           this.mapMode = !block ? MapMode.TrackDel : startSide <= 0 ? MapMode.TrackBefore : MapMode.TrackAfter;
       }
       // Only relevant when this.block == true
       get type() {
           return this.startSide < this.endSide ? BlockType.WidgetRange
               : this.startSide <= 0 ? BlockType.WidgetBefore : BlockType.WidgetAfter;
       }
       get heightRelevant() { return this.block || !!this.widget && this.widget.estimatedHeight >= 5; }
       eq(other) {
           return other instanceof PointDecoration &&
               widgetsEq(this.widget, other.widget) &&
               this.block == other.block &&
               this.startSide == other.startSide && this.endSide == other.endSide;
       }
       range(from, to = from) {
           if (this.isReplace && (from > to || (from == to && this.startSide > 0 && this.endSide <= 0)))
               throw new RangeError("Invalid range for replacement decoration");
           if (!this.isReplace && to != from)
               throw new RangeError("Widget decorations can only have zero-length ranges");
           return super.range(from, to);
       }
   }
   PointDecoration.prototype.point = true;
   function getInclusive(spec, block = false) {
       let { inclusiveStart: start, inclusiveEnd: end } = spec;
       if (start == null)
           start = spec.inclusive;
       if (end == null)
           end = spec.inclusive;
       return { start: start !== null && start !== void 0 ? start : block, end: end !== null && end !== void 0 ? end : block };
   }
   function widgetsEq(a, b) {
       return a == b || !!(a && b && a.compare(b));
   }
   function addRange(from, to, ranges, margin = 0) {
       let last = ranges.length - 1;
       if (last >= 0 && ranges[last] + margin >= from)
           ranges[last] = Math.max(ranges[last], to);
       else
           ranges.push(from, to);
   }

   class LineView extends ContentView {
       constructor() {
           super(...arguments);
           this.children = [];
           this.length = 0;
           this.prevAttrs = undefined;
           this.attrs = null;
           this.breakAfter = 0;
       }
       // Consumes source
       merge(from, to, source, hasStart, openStart, openEnd) {
           if (source) {
               if (!(source instanceof LineView))
                   return false;
               if (!this.dom)
                   source.transferDOM(this); // Reuse source.dom when appropriate
           }
           if (hasStart)
               this.setDeco(source ? source.attrs : null);
           mergeChildrenInto(this, from, to, source ? source.children : [], openStart, openEnd);
           return true;
       }
       split(at) {
           let end = new LineView;
           end.breakAfter = this.breakAfter;
           if (this.length == 0)
               return end;
           let { i, off } = this.childPos(at);
           if (off) {
               end.append(this.children[i].split(off), 0);
               this.children[i].merge(off, this.children[i].length, null, false, 0, 0);
               i++;
           }
           for (let j = i; j < this.children.length; j++)
               end.append(this.children[j], 0);
           while (i > 0 && this.children[i - 1].length == 0)
               this.children[--i].destroy();
           this.children.length = i;
           this.markDirty();
           this.length = at;
           return end;
       }
       transferDOM(other) {
           if (!this.dom)
               return;
           this.markDirty();
           other.setDOM(this.dom);
           other.prevAttrs = this.prevAttrs === undefined ? this.attrs : this.prevAttrs;
           this.prevAttrs = undefined;
           this.dom = null;
       }
       setDeco(attrs) {
           if (!attrsEq(this.attrs, attrs)) {
               if (this.dom) {
                   this.prevAttrs = this.attrs;
                   this.markDirty();
               }
               this.attrs = attrs;
           }
       }
       append(child, openStart) {
           joinInlineInto(this, child, openStart);
       }
       // Only called when building a line view in ContentBuilder
       addLineDeco(deco) {
           let attrs = deco.spec.attributes, cls = deco.spec.class;
           if (attrs)
               this.attrs = combineAttrs(attrs, this.attrs || {});
           if (cls)
               this.attrs = combineAttrs({ class: cls }, this.attrs || {});
       }
       domAtPos(pos) {
           return inlineDOMAtPos(this, pos);
       }
       reuseDOM(node) {
           if (node.nodeName == "DIV") {
               this.setDOM(node);
               this.dirty |= 4 /* Dirty.Attrs */ | 2 /* Dirty.Node */;
           }
       }
       sync(track) {
           var _a;
           if (!this.dom) {
               this.setDOM(document.createElement("div"));
               this.dom.className = "cm-line";
               this.prevAttrs = this.attrs ? null : undefined;
           }
           else if (this.dirty & 4 /* Dirty.Attrs */) {
               clearAttributes(this.dom);
               this.dom.className = "cm-line";
               this.prevAttrs = this.attrs ? null : undefined;
           }
           if (this.prevAttrs !== undefined) {
               updateAttrs(this.dom, this.prevAttrs, this.attrs);
               this.dom.classList.add("cm-line");
               this.prevAttrs = undefined;
           }
           super.sync(track);
           let last = this.dom.lastChild;
           while (last && ContentView.get(last) instanceof MarkView)
               last = last.lastChild;
           if (!last || !this.length ||
               last.nodeName != "BR" && ((_a = ContentView.get(last)) === null || _a === void 0 ? void 0 : _a.isEditable) == false &&
                   (!browser.ios || !this.children.some(ch => ch instanceof TextView))) {
               let hack = document.createElement("BR");
               hack.cmIgnore = true;
               this.dom.appendChild(hack);
           }
       }
       measureTextSize() {
           if (this.children.length == 0 || this.length > 20)
               return null;
           let totalWidth = 0;
           for (let child of this.children) {
               if (!(child instanceof TextView) || /[^ -~]/.test(child.text))
                   return null;
               let rects = clientRectsFor(child.dom);
               if (rects.length != 1)
                   return null;
               totalWidth += rects[0].width;
           }
           return !totalWidth ? null : {
               lineHeight: this.dom.getBoundingClientRect().height,
               charWidth: totalWidth / this.length
           };
       }
       coordsAt(pos, side) {
           return coordsInChildren(this, pos, side);
       }
       become(_other) { return false; }
       get type() { return BlockType.Text; }
       static find(docView, pos) {
           for (let i = 0, off = 0; i < docView.children.length; i++) {
               let block = docView.children[i], end = off + block.length;
               if (end >= pos) {
                   if (block instanceof LineView)
                       return block;
                   if (end > pos)
                       break;
               }
               off = end + block.breakAfter;
           }
           return null;
       }
   }
   class BlockWidgetView extends ContentView {
       constructor(widget, length, type) {
           super();
           this.widget = widget;
           this.length = length;
           this.type = type;
           this.breakAfter = 0;
           this.prevWidget = null;
       }
       merge(from, to, source, _takeDeco, openStart, openEnd) {
           if (source && (!(source instanceof BlockWidgetView) || !this.widget.compare(source.widget) ||
               from > 0 && openStart <= 0 || to < this.length && openEnd <= 0))
               return false;
           this.length = from + (source ? source.length : 0) + (this.length - to);
           return true;
       }
       domAtPos(pos) {
           return pos == 0 ? DOMPos.before(this.dom) : DOMPos.after(this.dom, pos == this.length);
       }
       split(at) {
           let len = this.length - at;
           this.length = at;
           let end = new BlockWidgetView(this.widget, len, this.type);
           end.breakAfter = this.breakAfter;
           return end;
       }
       get children() { return noChildren; }
       sync() {
           if (!this.dom || !this.widget.updateDOM(this.dom)) {
               if (this.dom && this.prevWidget)
                   this.prevWidget.destroy(this.dom);
               this.prevWidget = null;
               this.setDOM(this.widget.toDOM(this.editorView));
               this.dom.contentEditable = "false";
           }
       }
       get overrideDOMText() {
           return this.parent ? this.parent.view.state.doc.slice(this.posAtStart, this.posAtEnd) : Text.empty;
       }
       domBoundsAround() { return null; }
       become(other) {
           if (other instanceof BlockWidgetView && other.type == this.type &&
               other.widget.constructor == this.widget.constructor) {
               if (!other.widget.eq(this.widget))
                   this.markDirty(true);
               if (this.dom && !this.prevWidget)
                   this.prevWidget = this.widget;
               this.widget = other.widget;
               this.length = other.length;
               this.breakAfter = other.breakAfter;
               return true;
           }
           return false;
       }
       ignoreMutation() { return true; }
       ignoreEvent(event) { return this.widget.ignoreEvent(event); }
       destroy() {
           super.destroy();
           if (this.dom)
               this.widget.destroy(this.dom);
       }
   }

   class ContentBuilder {
       constructor(doc, pos, end, disallowBlockEffectsFor) {
           this.doc = doc;
           this.pos = pos;
           this.end = end;
           this.disallowBlockEffectsFor = disallowBlockEffectsFor;
           this.content = [];
           this.curLine = null;
           this.breakAtStart = 0;
           this.pendingBuffer = 0 /* Buf.No */;
           // Set to false directly after a widget that covers the position after it
           this.atCursorPos = true;
           this.openStart = -1;
           this.openEnd = -1;
           this.text = "";
           this.textOff = 0;
           this.cursor = doc.iter();
           this.skip = pos;
       }
       posCovered() {
           if (this.content.length == 0)
               return !this.breakAtStart && this.doc.lineAt(this.pos).from != this.pos;
           let last = this.content[this.content.length - 1];
           return !last.breakAfter && !(last instanceof BlockWidgetView && last.type == BlockType.WidgetBefore);
       }
       getLine() {
           if (!this.curLine) {
               this.content.push(this.curLine = new LineView);
               this.atCursorPos = true;
           }
           return this.curLine;
       }
       flushBuffer(active) {
           if (this.pendingBuffer) {
               this.curLine.append(wrapMarks(new WidgetBufferView(-1), active), active.length);
               this.pendingBuffer = 0 /* Buf.No */;
           }
       }
       addBlockWidget(view) {
           this.flushBuffer([]);
           this.curLine = null;
           this.content.push(view);
       }
       finish(openEnd) {
           if (!openEnd)
               this.flushBuffer([]);
           else
               this.pendingBuffer = 0 /* Buf.No */;
           if (!this.posCovered())
               this.getLine();
       }
       buildText(length, active, openStart) {
           while (length > 0) {
               if (this.textOff == this.text.length) {
                   let { value, lineBreak, done } = this.cursor.next(this.skip);
                   this.skip = 0;
                   if (done)
                       throw new Error("Ran out of text content when drawing inline views");
                   if (lineBreak) {
                       if (!this.posCovered())
                           this.getLine();
                       if (this.content.length)
                           this.content[this.content.length - 1].breakAfter = 1;
                       else
                           this.breakAtStart = 1;
                       this.flushBuffer([]);
                       this.curLine = null;
                       length--;
                       continue;
                   }
                   else {
                       this.text = value;
                       this.textOff = 0;
                   }
               }
               let take = Math.min(this.text.length - this.textOff, length, 512 /* T.Chunk */);
               this.flushBuffer(active.slice(0, openStart));
               this.getLine().append(wrapMarks(new TextView(this.text.slice(this.textOff, this.textOff + take)), active), openStart);
               this.atCursorPos = true;
               this.textOff += take;
               length -= take;
               openStart = 0;
           }
       }
       span(from, to, active, openStart) {
           this.buildText(to - from, active, openStart);
           this.pos = to;
           if (this.openStart < 0)
               this.openStart = openStart;
       }
       point(from, to, deco, active, openStart, index) {
           if (this.disallowBlockEffectsFor[index] && deco instanceof PointDecoration) {
               if (deco.block)
                   throw new RangeError("Block decorations may not be specified via plugins");
               if (to > this.doc.lineAt(this.pos).to)
                   throw new RangeError("Decorations that replace line breaks may not be specified via plugins");
           }
           let len = to - from;
           if (deco instanceof PointDecoration) {
               if (deco.block) {
                   let { type } = deco;
                   if (type == BlockType.WidgetAfter && !this.posCovered())
                       this.getLine();
                   this.addBlockWidget(new BlockWidgetView(deco.widget || new NullWidget("div"), len, type));
               }
               else {
                   let view = WidgetView.create(deco.widget || new NullWidget("span"), len, deco.startSide);
                   let cursorBefore = this.atCursorPos && !view.isEditable && openStart <= active.length && (from < to || deco.startSide > 0);
                   let cursorAfter = !view.isEditable && (from < to || deco.startSide <= 0);
                   let line = this.getLine();
                   if (this.pendingBuffer == 2 /* Buf.IfCursor */ && !cursorBefore)
                       this.pendingBuffer = 0 /* Buf.No */;
                   this.flushBuffer(active);
                   if (cursorBefore) {
                       line.append(wrapMarks(new WidgetBufferView(1), active), openStart);
                       openStart = active.length + Math.max(0, openStart - active.length);
                   }
                   line.append(wrapMarks(view, active), openStart);
                   this.atCursorPos = cursorAfter;
                   this.pendingBuffer = !cursorAfter ? 0 /* Buf.No */ : from < to ? 1 /* Buf.Yes */ : 2 /* Buf.IfCursor */;
               }
           }
           else if (this.doc.lineAt(this.pos).from == this.pos) { // Line decoration
               this.getLine().addLineDeco(deco);
           }
           if (len) {
               // Advance the iterator past the replaced content
               if (this.textOff + len <= this.text.length) {
                   this.textOff += len;
               }
               else {
                   this.skip += len - (this.text.length - this.textOff);
                   this.text = "";
                   this.textOff = 0;
               }
               this.pos = to;
           }
           if (this.openStart < 0)
               this.openStart = openStart;
       }
       static build(text, from, to, decorations, dynamicDecorationMap) {
           let builder = new ContentBuilder(text, from, to, dynamicDecorationMap);
           builder.openEnd = RangeSet.spans(decorations, from, to, builder);
           if (builder.openStart < 0)
               builder.openStart = builder.openEnd;
           builder.finish(builder.openEnd);
           return builder;
       }
   }
   function wrapMarks(view, active) {
       for (let mark of active)
           view = new MarkView(mark, [view], view.length);
       return view;
   }
   class NullWidget extends WidgetType {
       constructor(tag) {
           super();
           this.tag = tag;
       }
       eq(other) { return other.tag == this.tag; }
       toDOM() { return document.createElement(this.tag); }
       updateDOM(elt) { return elt.nodeName.toLowerCase() == this.tag; }
   }

   const clickAddsSelectionRange = /*@__PURE__*/Facet.define();
   const dragMovesSelection$1 = /*@__PURE__*/Facet.define();
   const mouseSelectionStyle = /*@__PURE__*/Facet.define();
   const exceptionSink = /*@__PURE__*/Facet.define();
   const updateListener = /*@__PURE__*/Facet.define();
   const inputHandler = /*@__PURE__*/Facet.define();
   const perLineTextDirection = /*@__PURE__*/Facet.define({
       combine: values => values.some(x => x)
   });
   class ScrollTarget {
       constructor(range, y = "nearest", x = "nearest", yMargin = 5, xMargin = 5) {
           this.range = range;
           this.y = y;
           this.x = x;
           this.yMargin = yMargin;
           this.xMargin = xMargin;
       }
       map(changes) {
           return changes.empty ? this : new ScrollTarget(this.range.map(changes), this.y, this.x, this.yMargin, this.xMargin);
       }
   }
   const scrollIntoView = /*@__PURE__*/StateEffect.define({ map: (t, ch) => t.map(ch) });
   /**
   Log or report an unhandled exception in client code. Should
   probably only be used by extension code that allows client code to
   provide functions, and calls those functions in a context where an
   exception can't be propagated to calling code in a reasonable way
   (for example when in an event handler).

   Either calls a handler registered with
   [`EditorView.exceptionSink`](https://codemirror.net/6/docs/ref/#view.EditorView^exceptionSink),
   `window.onerror`, if defined, or `console.error` (in which case
   it'll pass `context`, when given, as first argument).
   */
   function logException(state, exception, context) {
       let handler = state.facet(exceptionSink);
       if (handler.length)
           handler[0](exception);
       else if (window.onerror)
           window.onerror(String(exception), context, undefined, undefined, exception);
       else if (context)
           console.error(context + ":", exception);
       else
           console.error(exception);
   }
   const editable = /*@__PURE__*/Facet.define({ combine: values => values.length ? values[0] : true });
   let nextPluginID = 0;
   const viewPlugin = /*@__PURE__*/Facet.define();
   /**
   View plugins associate stateful values with a view. They can
   influence the way the content is drawn, and are notified of things
   that happen in the view.
   */
   class ViewPlugin {
       constructor(
       /**
       @internal
       */
       id, 
       /**
       @internal
       */
       create, 
       /**
       @internal
       */
       domEventHandlers, buildExtensions) {
           this.id = id;
           this.create = create;
           this.domEventHandlers = domEventHandlers;
           this.extension = buildExtensions(this);
       }
       /**
       Define a plugin from a constructor function that creates the
       plugin's value, given an editor view.
       */
       static define(create, spec) {
           const { eventHandlers, provide, decorations: deco } = spec || {};
           return new ViewPlugin(nextPluginID++, create, eventHandlers, plugin => {
               let ext = [viewPlugin.of(plugin)];
               if (deco)
                   ext.push(decorations.of(view => {
                       let pluginInst = view.plugin(plugin);
                       return pluginInst ? deco(pluginInst) : Decoration.none;
                   }));
               if (provide)
                   ext.push(provide(plugin));
               return ext;
           });
       }
       /**
       Create a plugin for a class whose constructor takes a single
       editor view as argument.
       */
       static fromClass(cls, spec) {
           return ViewPlugin.define(view => new cls(view), spec);
       }
   }
   class PluginInstance {
       constructor(spec) {
           this.spec = spec;
           // When starting an update, all plugins have this field set to the
           // update object, indicating they need to be updated. When finished
           // updating, it is set to `false`. Retrieving a plugin that needs to
           // be updated with `view.plugin` forces an eager update.
           this.mustUpdate = null;
           // This is null when the plugin is initially created, but
           // initialized on the first update.
           this.value = null;
       }
       update(view) {
           if (!this.value) {
               if (this.spec) {
                   try {
                       this.value = this.spec.create(view);
                   }
                   catch (e) {
                       logException(view.state, e, "CodeMirror plugin crashed");
                       this.deactivate();
                   }
               }
           }
           else if (this.mustUpdate) {
               let update = this.mustUpdate;
               this.mustUpdate = null;
               if (this.value.update) {
                   try {
                       this.value.update(update);
                   }
                   catch (e) {
                       logException(update.state, e, "CodeMirror plugin crashed");
                       if (this.value.destroy)
                           try {
                               this.value.destroy();
                           }
                           catch (_) { }
                       this.deactivate();
                   }
               }
           }
           return this;
       }
       destroy(view) {
           var _a;
           if ((_a = this.value) === null || _a === void 0 ? void 0 : _a.destroy) {
               try {
                   this.value.destroy();
               }
               catch (e) {
                   logException(view.state, e, "CodeMirror plugin crashed");
               }
           }
       }
       deactivate() {
           this.spec = this.value = null;
       }
   }
   const editorAttributes = /*@__PURE__*/Facet.define();
   const contentAttributes = /*@__PURE__*/Facet.define();
   // Provide decorations
   const decorations = /*@__PURE__*/Facet.define();
   const atomicRanges = /*@__PURE__*/Facet.define();
   const scrollMargins = /*@__PURE__*/Facet.define();
   const styleModule = /*@__PURE__*/Facet.define();
   class ChangedRange {
       constructor(fromA, toA, fromB, toB) {
           this.fromA = fromA;
           this.toA = toA;
           this.fromB = fromB;
           this.toB = toB;
       }
       join(other) {
           return new ChangedRange(Math.min(this.fromA, other.fromA), Math.max(this.toA, other.toA), Math.min(this.fromB, other.fromB), Math.max(this.toB, other.toB));
       }
       addToSet(set) {
           let i = set.length, me = this;
           for (; i > 0; i--) {
               let range = set[i - 1];
               if (range.fromA > me.toA)
                   continue;
               if (range.toA < me.fromA)
                   break;
               me = me.join(range);
               set.splice(i - 1, 1);
           }
           set.splice(i, 0, me);
           return set;
       }
       static extendWithRanges(diff, ranges) {
           if (ranges.length == 0)
               return diff;
           let result = [];
           for (let dI = 0, rI = 0, posA = 0, posB = 0;; dI++) {
               let next = dI == diff.length ? null : diff[dI], off = posA - posB;
               let end = next ? next.fromB : 1e9;
               while (rI < ranges.length && ranges[rI] < end) {
                   let from = ranges[rI], to = ranges[rI + 1];
                   let fromB = Math.max(posB, from), toB = Math.min(end, to);
                   if (fromB <= toB)
                       new ChangedRange(fromB + off, toB + off, fromB, toB).addToSet(result);
                   if (to > end)
                       break;
                   else
                       rI += 2;
               }
               if (!next)
                   return result;
               new ChangedRange(next.fromA, next.toA, next.fromB, next.toB).addToSet(result);
               posA = next.toA;
               posB = next.toB;
           }
       }
   }
   /**
   View [plugins](https://codemirror.net/6/docs/ref/#view.ViewPlugin) are given instances of this
   class, which describe what happened, whenever the view is updated.
   */
   class ViewUpdate {
       constructor(
       /**
       The editor view that the update is associated with.
       */
       view, 
       /**
       The new editor state.
       */
       state, 
       /**
       The transactions involved in the update. May be empty.
       */
       transactions) {
           this.view = view;
           this.state = state;
           this.transactions = transactions;
           /**
           @internal
           */
           this.flags = 0;
           this.startState = view.state;
           this.changes = ChangeSet.empty(this.startState.doc.length);
           for (let tr of transactions)
               this.changes = this.changes.compose(tr.changes);
           let changedRanges = [];
           this.changes.iterChangedRanges((fromA, toA, fromB, toB) => changedRanges.push(new ChangedRange(fromA, toA, fromB, toB)));
           this.changedRanges = changedRanges;
           let focus = view.hasFocus;
           if (focus != view.inputState.notifiedFocused) {
               view.inputState.notifiedFocused = focus;
               this.flags |= 1 /* UpdateFlag.Focus */;
           }
       }
       /**
       @internal
       */
       static create(view, state, transactions) {
           return new ViewUpdate(view, state, transactions);
       }
       /**
       Tells you whether the [viewport](https://codemirror.net/6/docs/ref/#view.EditorView.viewport) or
       [visible ranges](https://codemirror.net/6/docs/ref/#view.EditorView.visibleRanges) changed in this
       update.
       */
       get viewportChanged() {
           return (this.flags & 4 /* UpdateFlag.Viewport */) > 0;
       }
       /**
       Indicates whether the height of a block element in the editor
       changed in this update.
       */
       get heightChanged() {
           return (this.flags & 2 /* UpdateFlag.Height */) > 0;
       }
       /**
       Returns true when the document was modified or the size of the
       editor, or elements within the editor, changed.
       */
       get geometryChanged() {
           return this.docChanged || (this.flags & (8 /* UpdateFlag.Geometry */ | 2 /* UpdateFlag.Height */)) > 0;
       }
       /**
       True when this update indicates a focus change.
       */
       get focusChanged() {
           return (this.flags & 1 /* UpdateFlag.Focus */) > 0;
       }
       /**
       Whether the document changed in this update.
       */
       get docChanged() {
           return !this.changes.empty;
       }
       /**
       Whether the selection was explicitly set in this update.
       */
       get selectionSet() {
           return this.transactions.some(tr => tr.selection);
       }
       /**
       @internal
       */
       get empty() { return this.flags == 0 && this.transactions.length == 0; }
   }

   /**
   Used to indicate [text direction](https://codemirror.net/6/docs/ref/#view.EditorView.textDirection).
   */
   var Direction = /*@__PURE__*/(function (Direction) {
       // (These are chosen to match the base levels, in bidi algorithm
       // terms, of spans in that direction.)
       /**
       Left-to-right.
       */
       Direction[Direction["LTR"] = 0] = "LTR";
       /**
       Right-to-left.
       */
       Direction[Direction["RTL"] = 1] = "RTL";
   return Direction})(Direction || (Direction = {}));
   const LTR = Direction.LTR, RTL = Direction.RTL;
   // Decode a string with each type encoded as log2(type)
   function dec(str) {
       let result = [];
       for (let i = 0; i < str.length; i++)
           result.push(1 << +str[i]);
       return result;
   }
   // Character types for codepoints 0 to 0xf8
   const LowTypes = /*@__PURE__*/dec("88888888888888888888888888888888888666888888787833333333337888888000000000000000000000000008888880000000000000000000000000088888888888888888888888888888888888887866668888088888663380888308888800000000000000000000000800000000000000000000000000000008");
   // Character types for codepoints 0x600 to 0x6f9
   const ArabicTypes = /*@__PURE__*/dec("4444448826627288999999999992222222222222222222222222222222222222222222222229999999999999999999994444444444644222822222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222999999949999999229989999223333333333");
   const Brackets = /*@__PURE__*/Object.create(null), BracketStack = [];
   // There's a lot more in
   // https://www.unicode.org/Public/UCD/latest/ucd/BidiBrackets.txt,
   // which are left out to keep code size down.
   for (let p of ["()", "[]", "{}"]) {
       let l = /*@__PURE__*/p.charCodeAt(0), r = /*@__PURE__*/p.charCodeAt(1);
       Brackets[l] = r;
       Brackets[r] = -l;
   }
   function charType(ch) {
       return ch <= 0xf7 ? LowTypes[ch] :
           0x590 <= ch && ch <= 0x5f4 ? 2 /* T.R */ :
               0x600 <= ch && ch <= 0x6f9 ? ArabicTypes[ch - 0x600] :
                   0x6ee <= ch && ch <= 0x8ac ? 4 /* T.AL */ :
                       0x2000 <= ch && ch <= 0x200b ? 256 /* T.NI */ :
                           0xfb50 <= ch && ch <= 0xfdff ? 4 /* T.AL */ :
                               ch == 0x200c ? 256 /* T.NI */ : 1 /* T.L */;
   }
   const BidiRE = /[\u0590-\u05f4\u0600-\u06ff\u0700-\u08ac\ufb50-\ufdff]/;
   /**
   Represents a contiguous range of text that has a single direction
   (as in left-to-right or right-to-left).
   */
   class BidiSpan {
       /**
       @internal
       */
       constructor(
       /**
       The start of the span (relative to the start of the line).
       */
       from, 
       /**
       The end of the span.
       */
       to, 
       /**
       The ["bidi
       level"](https://unicode.org/reports/tr9/#Basic_Display_Algorithm)
       of the span (in this context, 0 means
       left-to-right, 1 means right-to-left, 2 means left-to-right
       number inside right-to-left text).
       */
       level) {
           this.from = from;
           this.to = to;
           this.level = level;
       }
       /**
       The direction of this span.
       */
       get dir() { return this.level % 2 ? RTL : LTR; }
       /**
       @internal
       */
       side(end, dir) { return (this.dir == dir) == end ? this.to : this.from; }
       /**
       @internal
       */
       static find(order, index, level, assoc) {
           let maybe = -1;
           for (let i = 0; i < order.length; i++) {
               let span = order[i];
               if (span.from <= index && span.to >= index) {
                   if (span.level == level)
                       return i;
                   // When multiple spans match, if assoc != 0, take the one that
                   // covers that side, otherwise take the one with the minimum
                   // level.
                   if (maybe < 0 || (assoc != 0 ? (assoc < 0 ? span.from < index : span.to > index) : order[maybe].level > span.level))
                       maybe = i;
               }
           }
           if (maybe < 0)
               throw new RangeError("Index out of range");
           return maybe;
       }
   }
   // Reused array of character types
   const types = [];
   function computeOrder(line, direction) {
       let len = line.length, outerType = direction == LTR ? 1 /* T.L */ : 2 /* T.R */, oppositeType = direction == LTR ? 2 /* T.R */ : 1 /* T.L */;
       if (!line || outerType == 1 /* T.L */ && !BidiRE.test(line))
           return trivialOrder(len);
       // W1. Examine each non-spacing mark (NSM) in the level run, and
       // change the type of the NSM to the type of the previous
       // character. If the NSM is at the start of the level run, it will
       // get the type of sor.
       // W2. Search backwards from each instance of a European number
       // until the first strong type (R, L, AL, or sor) is found. If an
       // AL is found, change the type of the European number to Arabic
       // number.
       // W3. Change all ALs to R.
       // (Left after this: L, R, EN, AN, ET, CS, NI)
       for (let i = 0, prev = outerType, prevStrong = outerType; i < len; i++) {
           let type = charType(line.charCodeAt(i));
           if (type == 512 /* T.NSM */)
               type = prev;
           else if (type == 8 /* T.EN */ && prevStrong == 4 /* T.AL */)
               type = 16 /* T.AN */;
           types[i] = type == 4 /* T.AL */ ? 2 /* T.R */ : type;
           if (type & 7 /* T.Strong */)
               prevStrong = type;
           prev = type;
       }
       // W5. A sequence of European terminators adjacent to European
       // numbers changes to all European numbers.
       // W6. Otherwise, separators and terminators change to Other
       // Neutral.
       // W7. Search backwards from each instance of a European number
       // until the first strong type (R, L, or sor) is found. If an L is
       // found, then change the type of the European number to L.
       // (Left after this: L, R, EN+AN, NI)
       for (let i = 0, prev = outerType, prevStrong = outerType; i < len; i++) {
           let type = types[i];
           if (type == 128 /* T.CS */) {
               if (i < len - 1 && prev == types[i + 1] && (prev & 24 /* T.Num */))
                   type = types[i] = prev;
               else
                   types[i] = 256 /* T.NI */;
           }
           else if (type == 64 /* T.ET */) {
               let end = i + 1;
               while (end < len && types[end] == 64 /* T.ET */)
                   end++;
               let replace = (i && prev == 8 /* T.EN */) || (end < len && types[end] == 8 /* T.EN */) ? (prevStrong == 1 /* T.L */ ? 1 /* T.L */ : 8 /* T.EN */) : 256 /* T.NI */;
               for (let j = i; j < end; j++)
                   types[j] = replace;
               i = end - 1;
           }
           else if (type == 8 /* T.EN */ && prevStrong == 1 /* T.L */) {
               types[i] = 1 /* T.L */;
           }
           prev = type;
           if (type & 7 /* T.Strong */)
               prevStrong = type;
       }
       // N0. Process bracket pairs in an isolating run sequence
       // sequentially in the logical order of the text positions of the
       // opening paired brackets using the logic given below. Within this
       // scope, bidirectional types EN and AN are treated as R.
       for (let i = 0, sI = 0, context = 0, ch, br, type; i < len; i++) {
           // Keeps [startIndex, type, strongSeen] triples for each open
           // bracket on BracketStack.
           if (br = Brackets[ch = line.charCodeAt(i)]) {
               if (br < 0) { // Closing bracket
                   for (let sJ = sI - 3; sJ >= 0; sJ -= 3) {
                       if (BracketStack[sJ + 1] == -br) {
                           let flags = BracketStack[sJ + 2];
                           let type = (flags & 2 /* Bracketed.EmbedInside */) ? outerType :
                               !(flags & 4 /* Bracketed.OppositeInside */) ? 0 :
                                   (flags & 1 /* Bracketed.OppositeBefore */) ? oppositeType : outerType;
                           if (type)
                               types[i] = types[BracketStack[sJ]] = type;
                           sI = sJ;
                           break;
                       }
                   }
               }
               else if (BracketStack.length == 189 /* Bracketed.MaxDepth */) {
                   break;
               }
               else {
                   BracketStack[sI++] = i;
                   BracketStack[sI++] = ch;
                   BracketStack[sI++] = context;
               }
           }
           else if ((type = types[i]) == 2 /* T.R */ || type == 1 /* T.L */) {
               let embed = type == outerType;
               context = embed ? 0 : 1 /* Bracketed.OppositeBefore */;
               for (let sJ = sI - 3; sJ >= 0; sJ -= 3) {
                   let cur = BracketStack[sJ + 2];
                   if (cur & 2 /* Bracketed.EmbedInside */)
                       break;
                   if (embed) {
                       BracketStack[sJ + 2] |= 2 /* Bracketed.EmbedInside */;
                   }
                   else {
                       if (cur & 4 /* Bracketed.OppositeInside */)
                           break;
                       BracketStack[sJ + 2] |= 4 /* Bracketed.OppositeInside */;
                   }
               }
           }
       }
       // N1. A sequence of neutrals takes the direction of the
       // surrounding strong text if the text on both sides has the same
       // direction. European and Arabic numbers act as if they were R in
       // terms of their influence on neutrals. Start-of-level-run (sor)
       // and end-of-level-run (eor) are used at level run boundaries.
       // N2. Any remaining neutrals take the embedding direction.
       // (Left after this: L, R, EN+AN)
       for (let i = 0; i < len; i++) {
           if (types[i] == 256 /* T.NI */) {
               let end = i + 1;
               while (end < len && types[end] == 256 /* T.NI */)
                   end++;
               let beforeL = (i ? types[i - 1] : outerType) == 1 /* T.L */;
               let afterL = (end < len ? types[end] : outerType) == 1 /* T.L */;
               let replace = beforeL == afterL ? (beforeL ? 1 /* T.L */ : 2 /* T.R */) : outerType;
               for (let j = i; j < end; j++)
                   types[j] = replace;
               i = end - 1;
           }
       }
       // Here we depart from the documented algorithm, in order to avoid
       // building up an actual levels array. Since there are only three
       // levels (0, 1, 2) in an implementation that doesn't take
       // explicit embedding into account, we can build up the order on
       // the fly, without following the level-based algorithm.
       let order = [];
       if (outerType == 1 /* T.L */) {
           for (let i = 0; i < len;) {
               let start = i, rtl = types[i++] != 1 /* T.L */;
               while (i < len && rtl == (types[i] != 1 /* T.L */))
                   i++;
               if (rtl) {
                   for (let j = i; j > start;) {
                       let end = j, l = types[--j] != 2 /* T.R */;
                       while (j > start && l == (types[j - 1] != 2 /* T.R */))
                           j--;
                       order.push(new BidiSpan(j, end, l ? 2 : 1));
                   }
               }
               else {
                   order.push(new BidiSpan(start, i, 0));
               }
           }
       }
       else {
           for (let i = 0; i < len;) {
               let start = i, rtl = types[i++] == 2 /* T.R */;
               while (i < len && rtl == (types[i] == 2 /* T.R */))
                   i++;
               order.push(new BidiSpan(start, i, rtl ? 1 : 2));
           }
       }
       return order;
   }
   function trivialOrder(length) {
       return [new BidiSpan(0, length, 0)];
   }
   let movedOver = "";
   function moveVisually(line, order, dir, start, forward) {
       var _a;
       let startIndex = start.head - line.from, spanI = -1;
       if (startIndex == 0) {
           if (!forward || !line.length)
               return null;
           if (order[0].level != dir) {
               startIndex = order[0].side(false, dir);
               spanI = 0;
           }
       }
       else if (startIndex == line.length) {
           if (forward)
               return null;
           let last = order[order.length - 1];
           if (last.level != dir) {
               startIndex = last.side(true, dir);
               spanI = order.length - 1;
           }
       }
       if (spanI < 0)
           spanI = BidiSpan.find(order, startIndex, (_a = start.bidiLevel) !== null && _a !== void 0 ? _a : -1, start.assoc);
       let span = order[spanI];
       // End of span. (But not end of line--that was checked for above.)
       if (startIndex == span.side(forward, dir)) {
           span = order[spanI += forward ? 1 : -1];
           startIndex = span.side(!forward, dir);
       }
       let indexForward = forward == (span.dir == dir);
       let nextIndex = findClusterBreak(line.text, startIndex, indexForward);
       movedOver = line.text.slice(Math.min(startIndex, nextIndex), Math.max(startIndex, nextIndex));
       if (nextIndex != span.side(forward, dir))
           return EditorSelection.cursor(nextIndex + line.from, indexForward ? -1 : 1, span.level);
       let nextSpan = spanI == (forward ? order.length - 1 : 0) ? null : order[spanI + (forward ? 1 : -1)];
       if (!nextSpan && span.level != dir)
           return EditorSelection.cursor(forward ? line.to : line.from, forward ? -1 : 1, dir);
       if (nextSpan && nextSpan.level < span.level)
           return EditorSelection.cursor(nextSpan.side(!forward, dir) + line.from, forward ? 1 : -1, nextSpan.level);
       return EditorSelection.cursor(nextIndex + line.from, forward ? -1 : 1, span.level);
   }

   const LineBreakPlaceholder = "\uffff";
   class DOMReader {
       constructor(points, state) {
           this.points = points;
           this.text = "";
           this.lineSeparator = state.facet(EditorState.lineSeparator);
       }
       append(text) {
           this.text += text;
       }
       lineBreak() {
           this.text += LineBreakPlaceholder;
       }
       readRange(start, end) {
           if (!start)
               return this;
           let parent = start.parentNode;
           for (let cur = start;;) {
               this.findPointBefore(parent, cur);
               this.readNode(cur);
               let next = cur.nextSibling;
               if (next == end)
                   break;
               let view = ContentView.get(cur), nextView = ContentView.get(next);
               if (view && nextView ? view.breakAfter :
                   (view ? view.breakAfter : isBlockElement(cur)) ||
                       (isBlockElement(next) && (cur.nodeName != "BR" || cur.cmIgnore)))
                   this.lineBreak();
               cur = next;
           }
           this.findPointBefore(parent, end);
           return this;
       }
       readTextNode(node) {
           let text = node.nodeValue;
           for (let point of this.points)
               if (point.node == node)
                   point.pos = this.text.length + Math.min(point.offset, text.length);
           for (let off = 0, re = this.lineSeparator ? null : /\r\n?|\n/g;;) {
               let nextBreak = -1, breakSize = 1, m;
               if (this.lineSeparator) {
                   nextBreak = text.indexOf(this.lineSeparator, off);
                   breakSize = this.lineSeparator.length;
               }
               else if (m = re.exec(text)) {
                   nextBreak = m.index;
                   breakSize = m[0].length;
               }
               this.append(text.slice(off, nextBreak < 0 ? text.length : nextBreak));
               if (nextBreak < 0)
                   break;
               this.lineBreak();
               if (breakSize > 1)
                   for (let point of this.points)
                       if (point.node == node && point.pos > this.text.length)
                           point.pos -= breakSize - 1;
               off = nextBreak + breakSize;
           }
       }
       readNode(node) {
           if (node.cmIgnore)
               return;
           let view = ContentView.get(node);
           let fromView = view && view.overrideDOMText;
           if (fromView != null) {
               this.findPointInside(node, fromView.length);
               for (let i = fromView.iter(); !i.next().done;) {
                   if (i.lineBreak)
                       this.lineBreak();
                   else
                       this.append(i.value);
               }
           }
           else if (node.nodeType == 3) {
               this.readTextNode(node);
           }
           else if (node.nodeName == "BR") {
               if (node.nextSibling)
                   this.lineBreak();
           }
           else if (node.nodeType == 1) {
               this.readRange(node.firstChild, null);
           }
       }
       findPointBefore(node, next) {
           for (let point of this.points)
               if (point.node == node && node.childNodes[point.offset] == next)
                   point.pos = this.text.length;
       }
       findPointInside(node, maxLen) {
           for (let point of this.points)
               if (node.nodeType == 3 ? point.node == node : node.contains(point.node))
                   point.pos = this.text.length + Math.min(maxLen, point.offset);
       }
   }
   function isBlockElement(node) {
       return node.nodeType == 1 && /^(DIV|P|LI|UL|OL|BLOCKQUOTE|DD|DT|H\d|SECTION|PRE)$/.test(node.nodeName);
   }
   class DOMPoint {
       constructor(node, offset) {
           this.node = node;
           this.offset = offset;
           this.pos = -1;
       }
   }

   class DocView extends ContentView {
       constructor(view) {
           super();
           this.view = view;
           this.compositionDeco = Decoration.none;
           this.decorations = [];
           this.dynamicDecorationMap = [];
           // Track a minimum width for the editor. When measuring sizes in
           // measureVisibleLineHeights, this is updated to point at the width
           // of a given element and its extent in the document. When a change
           // happens in that range, these are reset. That way, once we've seen
           // a line/element of a given length, we keep the editor wide enough
           // to fit at least that element, until it is changed, at which point
           // we forget it again.
           this.minWidth = 0;
           this.minWidthFrom = 0;
           this.minWidthTo = 0;
           // Track whether the DOM selection was set in a lossy way, so that
           // we don't mess it up when reading it back it
           this.impreciseAnchor = null;
           this.impreciseHead = null;
           this.forceSelection = false;
           // Used by the resize observer to ignore resizes that we caused
           // ourselves
           this.lastUpdate = Date.now();
           this.setDOM(view.contentDOM);
           this.children = [new LineView];
           this.children[0].setParent(this);
           this.updateDeco();
           this.updateInner([new ChangedRange(0, 0, 0, view.state.doc.length)], 0);
       }
       get editorView() { return this.view; }
       get length() { return this.view.state.doc.length; }
       // Update the document view to a given state. scrollIntoView can be
       // used as a hint to compute a new viewport that includes that
       // position, if we know the editor is going to scroll that position
       // into view.
       update(update) {
           let changedRanges = update.changedRanges;
           if (this.minWidth > 0 && changedRanges.length) {
               if (!changedRanges.every(({ fromA, toA }) => toA < this.minWidthFrom || fromA > this.minWidthTo)) {
                   this.minWidth = this.minWidthFrom = this.minWidthTo = 0;
               }
               else {
                   this.minWidthFrom = update.changes.mapPos(this.minWidthFrom, 1);
                   this.minWidthTo = update.changes.mapPos(this.minWidthTo, 1);
               }
           }
           if (this.view.inputState.composing < 0)
               this.compositionDeco = Decoration.none;
           else if (update.transactions.length || this.dirty)
               this.compositionDeco = computeCompositionDeco(this.view, update.changes);
           // When the DOM nodes around the selection are moved to another
           // parent, Chrome sometimes reports a different selection through
           // getSelection than the one that it actually shows to the user.
           // This forces a selection update when lines are joined to work
           // around that. Issue #54
           if ((browser.ie || browser.chrome) && !this.compositionDeco.size && update &&
               update.state.doc.lines != update.startState.doc.lines)
               this.forceSelection = true;
           let prevDeco = this.decorations, deco = this.updateDeco();
           let decoDiff = findChangedDeco(prevDeco, deco, update.changes);
           changedRanges = ChangedRange.extendWithRanges(changedRanges, decoDiff);
           if (this.dirty == 0 /* Dirty.Not */ && changedRanges.length == 0) {
               return false;
           }
           else {
               this.updateInner(changedRanges, update.startState.doc.length);
               if (update.transactions.length)
                   this.lastUpdate = Date.now();
               return true;
           }
       }
       // Used by update and the constructor do perform the actual DOM
       // update
       updateInner(changes, oldLength) {
           this.view.viewState.mustMeasureContent = true;
           this.updateChildren(changes, oldLength);
           let { observer } = this.view;
           observer.ignore(() => {
               // Lock the height during redrawing, since Chrome sometimes
               // messes with the scroll position during DOM mutation (though
               // no relayout is triggered and I cannot imagine how it can
               // recompute the scroll position without a layout)
               this.dom.style.height = this.view.viewState.contentHeight + "px";
               this.dom.style.flexBasis = this.minWidth ? this.minWidth + "px" : "";
               // Chrome will sometimes, when DOM mutations occur directly
               // around the selection, get confused and report a different
               // selection from the one it displays (issue #218). This tries
               // to detect that situation.
               let track = browser.chrome || browser.ios ? { node: observer.selectionRange.focusNode, written: false } : undefined;
               this.sync(track);
               this.dirty = 0 /* Dirty.Not */;
               if (track && (track.written || observer.selectionRange.focusNode != track.node))
                   this.forceSelection = true;
               this.dom.style.height = "";
           });
           let gaps = [];
           if (this.view.viewport.from || this.view.viewport.to < this.view.state.doc.length)
               for (let child of this.children)
                   if (child instanceof BlockWidgetView && child.widget instanceof BlockGapWidget)
                       gaps.push(child.dom);
           observer.updateGaps(gaps);
       }
       updateChildren(changes, oldLength) {
           let cursor = this.childCursor(oldLength);
           for (let i = changes.length - 1;; i--) {
               let next = i >= 0 ? changes[i] : null;
               if (!next)
                   break;
               let { fromA, toA, fromB, toB } = next;
               let { content, breakAtStart, openStart, openEnd } = ContentBuilder.build(this.view.state.doc, fromB, toB, this.decorations, this.dynamicDecorationMap);
               let { i: toI, off: toOff } = cursor.findPos(toA, 1);
               let { i: fromI, off: fromOff } = cursor.findPos(fromA, -1);
               replaceRange(this, fromI, fromOff, toI, toOff, content, breakAtStart, openStart, openEnd);
           }
       }
       // Sync the DOM selection to this.state.selection
       updateSelection(mustRead = false, fromPointer = false) {
           if (mustRead || !this.view.observer.selectionRange.focusNode)
               this.view.observer.readSelectionRange();
           if (!(fromPointer || this.mayControlSelection()))
               return;
           let force = this.forceSelection;
           this.forceSelection = false;
           let main = this.view.state.selection.main;
           // FIXME need to handle the case where the selection falls inside a block range
           let anchor = this.domAtPos(main.anchor);
           let head = main.empty ? anchor : this.domAtPos(main.head);
           // Always reset on Firefox when next to an uneditable node to
           // avoid invisible cursor bugs (#111)
           if (browser.gecko && main.empty && betweenUneditable(anchor)) {
               let dummy = document.createTextNode("");
               this.view.observer.ignore(() => anchor.node.insertBefore(dummy, anchor.node.childNodes[anchor.offset] || null));
               anchor = head = new DOMPos(dummy, 0);
               force = true;
           }
           let domSel = this.view.observer.selectionRange;
           // If the selection is already here, or in an equivalent position, don't touch it
           if (force || !domSel.focusNode ||
               !isEquivalentPosition(anchor.node, anchor.offset, domSel.anchorNode, domSel.anchorOffset) ||
               !isEquivalentPosition(head.node, head.offset, domSel.focusNode, domSel.focusOffset)) {
               this.view.observer.ignore(() => {
                   // Chrome Android will hide the virtual keyboard when tapping
                   // inside an uneditable node, and not bring it back when we
                   // move the cursor to its proper position. This tries to
                   // restore the keyboard by cycling focus.
                   if (browser.android && browser.chrome && this.dom.contains(domSel.focusNode) &&
                       inUneditable(domSel.focusNode, this.dom)) {
                       this.dom.blur();
                       this.dom.focus({ preventScroll: true });
                   }
                   let rawSel = getSelection(this.view.root);
                   if (!rawSel) ;
                   else if (main.empty) {
                       // Work around https://bugzilla.mozilla.org/show_bug.cgi?id=1612076
                       if (browser.gecko) {
                           let nextTo = nextToUneditable(anchor.node, anchor.offset);
                           if (nextTo && nextTo != (1 /* NextTo.Before */ | 2 /* NextTo.After */)) {
                               let text = nearbyTextNode(anchor.node, anchor.offset, nextTo == 1 /* NextTo.Before */ ? 1 : -1);
                               if (text)
                                   anchor = new DOMPos(text, nextTo == 1 /* NextTo.Before */ ? 0 : text.nodeValue.length);
                           }
                       }
                       rawSel.collapse(anchor.node, anchor.offset);
                       if (main.bidiLevel != null && domSel.cursorBidiLevel != null)
                           domSel.cursorBidiLevel = main.bidiLevel;
                   }
                   else if (rawSel.extend) {
                       // Selection.extend can be used to create an 'inverted' selection
                       // (one where the focus is before the anchor), but not all
                       // browsers support it yet.
                       rawSel.collapse(anchor.node, anchor.offset);
                       rawSel.extend(head.node, head.offset);
                   }
                   else {
                       // Primitive (IE) way
                       let range = document.createRange();
                       if (main.anchor > main.head)
                           [anchor, head] = [head, anchor];
                       range.setEnd(head.node, head.offset);
                       range.setStart(anchor.node, anchor.offset);
                       rawSel.removeAllRanges();
                       rawSel.addRange(range);
                   }
               });
               this.view.observer.setSelectionRange(anchor, head);
           }
           this.impreciseAnchor = anchor.precise ? null : new DOMPos(domSel.anchorNode, domSel.anchorOffset);
           this.impreciseHead = head.precise ? null : new DOMPos(domSel.focusNode, domSel.focusOffset);
       }
       enforceCursorAssoc() {
           if (this.compositionDeco.size)
               return;
           let cursor = this.view.state.selection.main;
           let sel = getSelection(this.view.root);
           if (!sel || !cursor.empty || !cursor.assoc || !sel.modify)
               return;
           let line = LineView.find(this, cursor.head);
           if (!line)
               return;
           let lineStart = line.posAtStart;
           if (cursor.head == lineStart || cursor.head == lineStart + line.length)
               return;
           let before = this.coordsAt(cursor.head, -1), after = this.coordsAt(cursor.head, 1);
           if (!before || !after || before.bottom > after.top)
               return;
           let dom = this.domAtPos(cursor.head + cursor.assoc);
           sel.collapse(dom.node, dom.offset);
           sel.modify("move", cursor.assoc < 0 ? "forward" : "backward", "lineboundary");
       }
       mayControlSelection() {
           let active = this.view.root.activeElement;
           return active == this.dom ||
               hasSelection(this.dom, this.view.observer.selectionRange) && !(active && this.dom.contains(active));
       }
       nearest(dom) {
           for (let cur = dom; cur;) {
               let domView = ContentView.get(cur);
               if (domView && domView.rootView == this)
                   return domView;
               cur = cur.parentNode;
           }
           return null;
       }
       posFromDOM(node, offset) {
           let view = this.nearest(node);
           if (!view)
               throw new RangeError("Trying to find position for a DOM position outside of the document");
           return view.localPosFromDOM(node, offset) + view.posAtStart;
       }
       domAtPos(pos) {
           let { i, off } = this.childCursor().findPos(pos, -1);
           for (; i < this.children.length - 1;) {
               let child = this.children[i];
               if (off < child.length || child instanceof LineView)
                   break;
               i++;
               off = 0;
           }
           return this.children[i].domAtPos(off);
       }
       coordsAt(pos, side) {
           for (let off = this.length, i = this.children.length - 1;; i--) {
               let child = this.children[i], start = off - child.breakAfter - child.length;
               if (pos > start ||
                   (pos == start && child.type != BlockType.WidgetBefore && child.type != BlockType.WidgetAfter &&
                       (!i || side == 2 || this.children[i - 1].breakAfter ||
                           (this.children[i - 1].type == BlockType.WidgetBefore && side > -2))))
                   return child.coordsAt(pos - start, side);
               off = start;
           }
       }
       measureVisibleLineHeights(viewport) {
           let result = [], { from, to } = viewport;
           let contentWidth = this.view.contentDOM.clientWidth;
           let isWider = contentWidth > Math.max(this.view.scrollDOM.clientWidth, this.minWidth) + 1;
           let widest = -1, ltr = this.view.textDirection == Direction.LTR;
           for (let pos = 0, i = 0; i < this.children.length; i++) {
               let child = this.children[i], end = pos + child.length;
               if (end > to)
                   break;
               if (pos >= from) {
                   let childRect = child.dom.getBoundingClientRect();
                   result.push(childRect.height);
                   if (isWider) {
                       let last = child.dom.lastChild;
                       let rects = last ? clientRectsFor(last) : [];
                       if (rects.length) {
                           let rect = rects[rects.length - 1];
                           let width = ltr ? rect.right - childRect.left : childRect.right - rect.left;
                           if (width > widest) {
                               widest = width;
                               this.minWidth = contentWidth;
                               this.minWidthFrom = pos;
                               this.minWidthTo = end;
                           }
                       }
                   }
               }
               pos = end + child.breakAfter;
           }
           return result;
       }
       textDirectionAt(pos) {
           let { i } = this.childPos(pos, 1);
           return getComputedStyle(this.children[i].dom).direction == "rtl" ? Direction.RTL : Direction.LTR;
       }
       measureTextSize() {
           for (let child of this.children) {
               if (child instanceof LineView) {
                   let measure = child.measureTextSize();
                   if (measure)
                       return measure;
               }
           }
           // If no workable line exists, force a layout of a measurable element
           let dummy = document.createElement("div"), lineHeight, charWidth;
           dummy.className = "cm-line";
           dummy.style.width = "99999px";
           dummy.textContent = "abc def ghi jkl mno pqr stu";
           this.view.observer.ignore(() => {
               this.dom.appendChild(dummy);
               let rect = clientRectsFor(dummy.firstChild)[0];
               lineHeight = dummy.getBoundingClientRect().height;
               charWidth = rect ? rect.width / 27 : 7;
               dummy.remove();
           });
           return { lineHeight, charWidth };
       }
       childCursor(pos = this.length) {
           // Move back to start of last element when possible, so that
           // `ChildCursor.findPos` doesn't have to deal with the edge case
           // of being after the last element.
           let i = this.children.length;
           if (i)
               pos -= this.children[--i].length;
           return new ChildCursor(this.children, pos, i);
       }
       computeBlockGapDeco() {
           let deco = [], vs = this.view.viewState;
           for (let pos = 0, i = 0;; i++) {
               let next = i == vs.viewports.length ? null : vs.viewports[i];
               let end = next ? next.from - 1 : this.length;
               if (end > pos) {
                   let height = vs.lineBlockAt(end).bottom - vs.lineBlockAt(pos).top;
                   deco.push(Decoration.replace({
                       widget: new BlockGapWidget(height),
                       block: true,
                       inclusive: true,
                       isBlockGap: true,
                   }).range(pos, end));
               }
               if (!next)
                   break;
               pos = next.to + 1;
           }
           return Decoration.set(deco);
       }
       updateDeco() {
           let allDeco = this.view.state.facet(decorations).map((d, i) => {
               let dynamic = this.dynamicDecorationMap[i] = typeof d == "function";
               return dynamic ? d(this.view) : d;
           });
           for (let i = allDeco.length; i < allDeco.length + 3; i++)
               this.dynamicDecorationMap[i] = false;
           return this.decorations = [
               ...allDeco,
               this.compositionDeco,
               this.computeBlockGapDeco(),
               this.view.viewState.lineGapDeco
           ];
       }
       scrollIntoView(target) {
           let { range } = target;
           let rect = this.coordsAt(range.head, range.empty ? range.assoc : range.head > range.anchor ? -1 : 1), other;
           if (!rect)
               return;
           if (!range.empty && (other = this.coordsAt(range.anchor, range.anchor > range.head ? -1 : 1)))
               rect = { left: Math.min(rect.left, other.left), top: Math.min(rect.top, other.top),
                   right: Math.max(rect.right, other.right), bottom: Math.max(rect.bottom, other.bottom) };
           let mLeft = 0, mRight = 0, mTop = 0, mBottom = 0;
           for (let margins of this.view.state.facet(scrollMargins).map(f => f(this.view)))
               if (margins) {
                   let { left, right, top, bottom } = margins;
                   if (left != null)
                       mLeft = Math.max(mLeft, left);
                   if (right != null)
                       mRight = Math.max(mRight, right);
                   if (top != null)
                       mTop = Math.max(mTop, top);
                   if (bottom != null)
                       mBottom = Math.max(mBottom, bottom);
               }
           let targetRect = {
               left: rect.left - mLeft, top: rect.top - mTop,
               right: rect.right + mRight, bottom: rect.bottom + mBottom
           };
           scrollRectIntoView(this.view.scrollDOM, targetRect, range.head < range.anchor ? -1 : 1, target.x, target.y, target.xMargin, target.yMargin, this.view.textDirection == Direction.LTR);
       }
   }
   function betweenUneditable(pos) {
       return pos.node.nodeType == 1 && pos.node.firstChild &&
           (pos.offset == 0 || pos.node.childNodes[pos.offset - 1].contentEditable == "false") &&
           (pos.offset == pos.node.childNodes.length || pos.node.childNodes[pos.offset].contentEditable == "false");
   }
   class BlockGapWidget extends WidgetType {
       constructor(height) {
           super();
           this.height = height;
       }
       toDOM() {
           let elt = document.createElement("div");
           this.updateDOM(elt);
           return elt;
       }
       eq(other) { return other.height == this.height; }
       updateDOM(elt) {
           elt.style.height = this.height + "px";
           return true;
       }
       get estimatedHeight() { return this.height; }
   }
   function compositionSurroundingNode(view) {
       let sel = view.observer.selectionRange;
       let textNode = sel.focusNode && nearbyTextNode(sel.focusNode, sel.focusOffset, 0);
       if (!textNode)
           return null;
       let cView = view.docView.nearest(textNode);
       if (!cView)
           return null;
       if (cView instanceof LineView) {
           let topNode = textNode;
           while (topNode.parentNode != cView.dom)
               topNode = topNode.parentNode;
           let prev = topNode.previousSibling;
           while (prev && !ContentView.get(prev))
               prev = prev.previousSibling;
           let pos = prev ? ContentView.get(prev).posAtEnd : cView.posAtStart;
           return { from: pos, to: pos, node: topNode, text: textNode };
       }
       else {
           for (;;) {
               let { parent } = cView;
               if (!parent)
                   return null;
               if (parent instanceof LineView)
                   break;
               cView = parent;
           }
           let from = cView.posAtStart;
           return { from, to: from + cView.length, node: cView.dom, text: textNode };
       }
   }
   function computeCompositionDeco(view, changes) {
       let surrounding = compositionSurroundingNode(view);
       if (!surrounding)
           return Decoration.none;
       let { from, to, node, text: textNode } = surrounding;
       let newFrom = changes.mapPos(from, 1), newTo = Math.max(newFrom, changes.mapPos(to, -1));
       let { state } = view, text = node.nodeType == 3 ? node.nodeValue :
           new DOMReader([], state).readRange(node.firstChild, null).text;
       if (newTo - newFrom < text.length) {
           if (state.doc.sliceString(newFrom, Math.min(state.doc.length, newFrom + text.length), LineBreakPlaceholder) == text)
               newTo = newFrom + text.length;
           else if (state.doc.sliceString(Math.max(0, newTo - text.length), newTo, LineBreakPlaceholder) == text)
               newFrom = newTo - text.length;
           else
               return Decoration.none;
       }
       else if (state.doc.sliceString(newFrom, newTo, LineBreakPlaceholder) != text) {
           return Decoration.none;
       }
       let topView = ContentView.get(node);
       if (topView instanceof CompositionView)
           topView = topView.widget.topView;
       else if (topView)
           topView.parent = null;
       return Decoration.set(Decoration.replace({ widget: new CompositionWidget(node, textNode, topView), inclusive: true })
           .range(newFrom, newTo));
   }
   class CompositionWidget extends WidgetType {
       constructor(top, text, topView) {
           super();
           this.top = top;
           this.text = text;
           this.topView = topView;
       }
       eq(other) { return this.top == other.top && this.text == other.text; }
       toDOM() { return this.top; }
       ignoreEvent() { return false; }
       get customView() { return CompositionView; }
   }
   function nearbyTextNode(node, offset, side) {
       for (;;) {
           if (node.nodeType == 3)
               return node;
           if (node.nodeType == 1 && offset > 0 && side <= 0) {
               node = node.childNodes[offset - 1];
               offset = maxOffset(node);
           }
           else if (node.nodeType == 1 && offset < node.childNodes.length && side >= 0) {
               node = node.childNodes[offset];
               offset = 0;
           }
           else {
               return null;
           }
       }
   }
   function nextToUneditable(node, offset) {
       if (node.nodeType != 1)
           return 0;
       return (offset && node.childNodes[offset - 1].contentEditable == "false" ? 1 /* NextTo.Before */ : 0) |
           (offset < node.childNodes.length && node.childNodes[offset].contentEditable == "false" ? 2 /* NextTo.After */ : 0);
   }
   class DecorationComparator$1 {
       constructor() {
           this.changes = [];
       }
       compareRange(from, to) { addRange(from, to, this.changes); }
       comparePoint(from, to) { addRange(from, to, this.changes); }
   }
   function findChangedDeco(a, b, diff) {
       let comp = new DecorationComparator$1;
       RangeSet.compare(a, b, diff, comp);
       return comp.changes;
   }
   function inUneditable(node, inside) {
       for (let cur = node; cur && cur != inside; cur = cur.assignedSlot || cur.parentNode) {
           if (cur.nodeType == 1 && cur.contentEditable == 'false') {
               return true;
           }
       }
       return false;
   }

   function groupAt(state, pos, bias = 1) {
       let categorize = state.charCategorizer(pos);
       let line = state.doc.lineAt(pos), linePos = pos - line.from;
       if (line.length == 0)
           return EditorSelection.cursor(pos);
       if (linePos == 0)
           bias = 1;
       else if (linePos == line.length)
           bias = -1;
       let from = linePos, to = linePos;
       if (bias < 0)
           from = findClusterBreak(line.text, linePos, false);
       else
           to = findClusterBreak(line.text, linePos);
       let cat = categorize(line.text.slice(from, to));
       while (from > 0) {
           let prev = findClusterBreak(line.text, from, false);
           if (categorize(line.text.slice(prev, from)) != cat)
               break;
           from = prev;
       }
       while (to < line.length) {
           let next = findClusterBreak(line.text, to);
           if (categorize(line.text.slice(to, next)) != cat)
               break;
           to = next;
       }
       return EditorSelection.range(from + line.from, to + line.from);
   }
   // Search the DOM for the {node, offset} position closest to the given
   // coordinates. Very inefficient and crude, but can usually be avoided
   // by calling caret(Position|Range)FromPoint instead.
   function getdx(x, rect) {
       return rect.left > x ? rect.left - x : Math.max(0, x - rect.right);
   }
   function getdy(y, rect) {
       return rect.top > y ? rect.top - y : Math.max(0, y - rect.bottom);
   }
   function yOverlap(a, b) {
       return a.top < b.bottom - 1 && a.bottom > b.top + 1;
   }
   function upTop(rect, top) {
       return top < rect.top ? { top, left: rect.left, right: rect.right, bottom: rect.bottom } : rect;
   }
   function upBot(rect, bottom) {
       return bottom > rect.bottom ? { top: rect.top, left: rect.left, right: rect.right, bottom } : rect;
   }
   function domPosAtCoords(parent, x, y) {
       let closest, closestRect, closestX, closestY, closestOverlap = false;
       let above, below, aboveRect, belowRect;
       for (let child = parent.firstChild; child; child = child.nextSibling) {
           let rects = clientRectsFor(child);
           for (let i = 0; i < rects.length; i++) {
               let rect = rects[i];
               if (closestRect && yOverlap(closestRect, rect))
                   rect = upTop(upBot(rect, closestRect.bottom), closestRect.top);
               let dx = getdx(x, rect), dy = getdy(y, rect);
               if (dx == 0 && dy == 0)
                   return child.nodeType == 3 ? domPosInText(child, x, y) : domPosAtCoords(child, x, y);
               if (!closest || closestY > dy || closestY == dy && closestX > dx) {
                   closest = child;
                   closestRect = rect;
                   closestX = dx;
                   closestY = dy;
                   closestOverlap = !dx || (dx > 0 ? i < rects.length - 1 : i > 0);
               }
               if (dx == 0) {
                   if (y > rect.bottom && (!aboveRect || aboveRect.bottom < rect.bottom)) {
                       above = child;
                       aboveRect = rect;
                   }
                   else if (y < rect.top && (!belowRect || belowRect.top > rect.top)) {
                       below = child;
                       belowRect = rect;
                   }
               }
               else if (aboveRect && yOverlap(aboveRect, rect)) {
                   aboveRect = upBot(aboveRect, rect.bottom);
               }
               else if (belowRect && yOverlap(belowRect, rect)) {
                   belowRect = upTop(belowRect, rect.top);
               }
           }
       }
       if (aboveRect && aboveRect.bottom >= y) {
           closest = above;
           closestRect = aboveRect;
       }
       else if (belowRect && belowRect.top <= y) {
           closest = below;
           closestRect = belowRect;
       }
       if (!closest)
           return { node: parent, offset: 0 };
       let clipX = Math.max(closestRect.left, Math.min(closestRect.right, x));
       if (closest.nodeType == 3)
           return domPosInText(closest, clipX, y);
       if (closestOverlap && closest.contentEditable != "false")
           return domPosAtCoords(closest, clipX, y);
       let offset = Array.prototype.indexOf.call(parent.childNodes, closest) +
           (x >= (closestRect.left + closestRect.right) / 2 ? 1 : 0);
       return { node: parent, offset };
   }
   function domPosInText(node, x, y) {
       let len = node.nodeValue.length;
       let closestOffset = -1, closestDY = 1e9, generalSide = 0;
       for (let i = 0; i < len; i++) {
           let rects = textRange(node, i, i + 1).getClientRects();
           for (let j = 0; j < rects.length; j++) {
               let rect = rects[j];
               if (rect.top == rect.bottom)
                   continue;
               if (!generalSide)
                   generalSide = x - rect.left;
               let dy = (rect.top > y ? rect.top - y : y - rect.bottom) - 1;
               if (rect.left - 1 <= x && rect.right + 1 >= x && dy < closestDY) {
                   let right = x >= (rect.left + rect.right) / 2, after = right;
                   if (browser.chrome || browser.gecko) {
                       // Check for RTL on browsers that support getting client
                       // rects for empty ranges.
                       let rectBefore = textRange(node, i).getBoundingClientRect();
                       if (rectBefore.left == rect.right)
                           after = !right;
                   }
                   if (dy <= 0)
                       return { node, offset: i + (after ? 1 : 0) };
                   closestOffset = i + (after ? 1 : 0);
                   closestDY = dy;
               }
           }
       }
       return { node, offset: closestOffset > -1 ? closestOffset : generalSide > 0 ? node.nodeValue.length : 0 };
   }
   function posAtCoords(view, { x, y }, precise, bias = -1) {
       var _a;
       let content = view.contentDOM.getBoundingClientRect(), docTop = content.top + view.viewState.paddingTop;
       let block, { docHeight } = view.viewState;
       let yOffset = y - docTop;
       if (yOffset < 0)
           return 0;
       if (yOffset > docHeight)
           return view.state.doc.length;
       // Scan for a text block near the queried y position
       for (let halfLine = view.defaultLineHeight / 2, bounced = false;;) {
           block = view.elementAtHeight(yOffset);
           if (block.type == BlockType.Text)
               break;
           for (;;) {
               // Move the y position out of this block
               yOffset = bias > 0 ? block.bottom + halfLine : block.top - halfLine;
               if (yOffset >= 0 && yOffset <= docHeight)
                   break;
               // If the document consists entirely of replaced widgets, we
               // won't find a text block, so return 0
               if (bounced)
                   return precise ? null : 0;
               bounced = true;
               bias = -bias;
           }
       }
       y = docTop + yOffset;
       let lineStart = block.from;
       // If this is outside of the rendered viewport, we can't determine a position
       if (lineStart < view.viewport.from)
           return view.viewport.from == 0 ? 0 : precise ? null : posAtCoordsImprecise(view, content, block, x, y);
       if (lineStart > view.viewport.to)
           return view.viewport.to == view.state.doc.length ? view.state.doc.length :
               precise ? null : posAtCoordsImprecise(view, content, block, x, y);
       // Prefer ShadowRootOrDocument.elementFromPoint if present, fall back to document if not
       let doc = view.dom.ownerDocument;
       let root = view.root.elementFromPoint ? view.root : doc;
       let element = root.elementFromPoint(x, y);
       if (element && !view.contentDOM.contains(element))
           element = null;
       // If the element is unexpected, clip x at the sides of the content area and try again
       if (!element) {
           x = Math.max(content.left + 1, Math.min(content.right - 1, x));
           element = root.elementFromPoint(x, y);
           if (element && !view.contentDOM.contains(element))
               element = null;
       }
       // There's visible editor content under the point, so we can try
       // using caret(Position|Range)FromPoint as a shortcut
       let node, offset = -1;
       if (element && ((_a = view.docView.nearest(element)) === null || _a === void 0 ? void 0 : _a.isEditable) != false) {
           if (doc.caretPositionFromPoint) {
               let pos = doc.caretPositionFromPoint(x, y);
               if (pos)
                   ({ offsetNode: node, offset } = pos);
           }
           else if (doc.caretRangeFromPoint) {
               let range = doc.caretRangeFromPoint(x, y);
               if (range) {
                   ({ startContainer: node, startOffset: offset } = range);
                   if (!view.contentDOM.contains(node) ||
                       browser.safari && isSuspiciousSafariCaretResult(node, offset, x) ||
                       browser.chrome && isSuspiciousChromeCaretResult(node, offset, x))
                       node = undefined;
               }
           }
       }
       // No luck, do our own (potentially expensive) search
       if (!node || !view.docView.dom.contains(node)) {
           let line = LineView.find(view.docView, lineStart);
           if (!line)
               return yOffset > block.top + block.height / 2 ? block.to : block.from;
           ({ node, offset } = domPosAtCoords(line.dom, x, y));
       }
       return view.docView.posFromDOM(node, offset);
   }
   function posAtCoordsImprecise(view, contentRect, block, x, y) {
       let into = Math.round((x - contentRect.left) * view.defaultCharacterWidth);
       if (view.lineWrapping && block.height > view.defaultLineHeight * 1.5) {
           let line = Math.floor((y - block.top) / view.defaultLineHeight);
           into += line * view.viewState.heightOracle.lineLength;
       }
       let content = view.state.sliceDoc(block.from, block.to);
       return block.from + findColumn(content, into, view.state.tabSize);
   }
   // In case of a high line height, Safari's caretRangeFromPoint treats
   // the space between lines as belonging to the last character of the
   // line before. This is used to detect such a result so that it can be
   // ignored (issue #401).
   function isSuspiciousSafariCaretResult(node, offset, x) {
       let len;
       if (node.nodeType != 3 || offset != (len = node.nodeValue.length))
           return false;
       for (let next = node.nextSibling; next; next = next.nextSibling)
           if (next.nodeType != 1 || next.nodeName != "BR")
               return false;
       return textRange(node, len - 1, len).getBoundingClientRect().left > x;
   }
   // Chrome will move positions between lines to the start of the next line
   function isSuspiciousChromeCaretResult(node, offset, x) {
       if (offset != 0)
           return false;
       for (let cur = node;;) {
           let parent = cur.parentNode;
           if (!parent || parent.nodeType != 1 || parent.firstChild != cur)
               return false;
           if (parent.classList.contains("cm-line"))
               break;
           cur = parent;
       }
       let rect = node.nodeType == 1 ? node.getBoundingClientRect()
           : textRange(node, 0, Math.max(node.nodeValue.length, 1)).getBoundingClientRect();
       return x - rect.left > 5;
   }
   function moveToLineBoundary(view, start, forward, includeWrap) {
       let line = view.state.doc.lineAt(start.head);
       let coords = !includeWrap || !view.lineWrapping ? null
           : view.coordsAtPos(start.assoc < 0 && start.head > line.from ? start.head - 1 : start.head);
       if (coords) {
           let editorRect = view.dom.getBoundingClientRect();
           let direction = view.textDirectionAt(line.from);
           let pos = view.posAtCoords({ x: forward == (direction == Direction.LTR) ? editorRect.right - 1 : editorRect.left + 1,
               y: (coords.top + coords.bottom) / 2 });
           if (pos != null)
               return EditorSelection.cursor(pos, forward ? -1 : 1);
       }
       let lineView = LineView.find(view.docView, start.head);
       let end = lineView ? (forward ? lineView.posAtEnd : lineView.posAtStart) : (forward ? line.to : line.from);
       return EditorSelection.cursor(end, forward ? -1 : 1);
   }
   function moveByChar(view, start, forward, by) {
       let line = view.state.doc.lineAt(start.head), spans = view.bidiSpans(line);
       let direction = view.textDirectionAt(line.from);
       for (let cur = start, check = null;;) {
           let next = moveVisually(line, spans, direction, cur, forward), char = movedOver;
           if (!next) {
               if (line.number == (forward ? view.state.doc.lines : 1))
                   return cur;
               char = "\n";
               line = view.state.doc.line(line.number + (forward ? 1 : -1));
               spans = view.bidiSpans(line);
               next = EditorSelection.cursor(forward ? line.from : line.to);
           }
           if (!check) {
               if (!by)
                   return next;
               check = by(char);
           }
           else if (!check(char)) {
               return cur;
           }
           cur = next;
       }
   }
   function byGroup(view, pos, start) {
       let categorize = view.state.charCategorizer(pos);
       let cat = categorize(start);
       return (next) => {
           let nextCat = categorize(next);
           if (cat == CharCategory.Space)
               cat = nextCat;
           return cat == nextCat;
       };
   }
   function moveVertically(view, start, forward, distance) {
       let startPos = start.head, dir = forward ? 1 : -1;
       if (startPos == (forward ? view.state.doc.length : 0))
           return EditorSelection.cursor(startPos, start.assoc);
       let goal = start.goalColumn, startY;
       let rect = view.contentDOM.getBoundingClientRect();
       let startCoords = view.coordsAtPos(startPos), docTop = view.documentTop;
       if (startCoords) {
           if (goal == null)
               goal = startCoords.left - rect.left;
           startY = dir < 0 ? startCoords.top : startCoords.bottom;
       }
       else {
           let line = view.viewState.lineBlockAt(startPos);
           if (goal == null)
               goal = Math.min(rect.right - rect.left, view.defaultCharacterWidth * (startPos - line.from));
           startY = (dir < 0 ? line.top : line.bottom) + docTop;
       }
       let resolvedGoal = rect.left + goal;
       let dist = distance !== null && distance !== void 0 ? distance : (view.defaultLineHeight >> 1);
       for (let extra = 0;; extra += 10) {
           let curY = startY + (dist + extra) * dir;
           let pos = posAtCoords(view, { x: resolvedGoal, y: curY }, false, dir);
           if (curY < rect.top || curY > rect.bottom || (dir < 0 ? pos < startPos : pos > startPos))
               return EditorSelection.cursor(pos, start.assoc, undefined, goal);
       }
   }
   function skipAtoms(view, oldPos, pos) {
       let atoms = view.state.facet(atomicRanges).map(f => f(view));
       for (;;) {
           let moved = false;
           for (let set of atoms) {
               set.between(pos.from - 1, pos.from + 1, (from, to, value) => {
                   if (pos.from > from && pos.from < to) {
                       pos = oldPos.head > pos.from ? EditorSelection.cursor(from, 1) : EditorSelection.cursor(to, -1);
                       moved = true;
                   }
               });
           }
           if (!moved)
               return pos;
       }
   }

   // This will also be where dragging info and such goes
   class InputState {
       constructor(view) {
           this.lastKeyCode = 0;
           this.lastKeyTime = 0;
           this.lastTouchTime = 0;
           this.lastFocusTime = 0;
           this.lastScrollTop = 0;
           this.lastScrollLeft = 0;
           this.chromeScrollHack = -1;
           // On iOS, some keys need to have their default behavior happen
           // (after which we retroactively handle them and reset the DOM) to
           // avoid messing up the virtual keyboard state.
           this.pendingIOSKey = undefined;
           this.lastSelectionOrigin = null;
           this.lastSelectionTime = 0;
           this.lastEscPress = 0;
           this.lastContextMenu = 0;
           this.scrollHandlers = [];
           this.registeredEvents = [];
           this.customHandlers = [];
           // -1 means not in a composition. Otherwise, this counts the number
           // of changes made during the composition. The count is used to
           // avoid treating the start state of the composition, before any
           // changes have been made, as part of the composition.
           this.composing = -1;
           // Tracks whether the next change should be marked as starting the
           // composition (null means no composition, true means next is the
           // first, false means first has already been marked for this
           // composition)
           this.compositionFirstChange = null;
           this.compositionEndedAt = 0;
           this.mouseSelection = null;
           for (let type in handlers) {
               let handler = handlers[type];
               view.contentDOM.addEventListener(type, (event) => {
                   if (!eventBelongsToEditor(view, event) || this.ignoreDuringComposition(event))
                       return;
                   if (type == "keydown" && this.keydown(view, event))
                       return;
                   if (this.mustFlushObserver(event))
                       view.observer.forceFlush();
                   if (this.runCustomHandlers(type, view, event))
                       event.preventDefault();
                   else
                       handler(view, event);
               }, handlerOptions[type]);
               this.registeredEvents.push(type);
           }
           if (browser.chrome && browser.chrome_version == 102) { // FIXME remove at some point
               // On Chrome 102, viewport updates somehow stop wheel-based
               // scrolling. Turning off pointer events during the scroll seems
               // to avoid the issue.
               view.scrollDOM.addEventListener("wheel", () => {
                   if (this.chromeScrollHack < 0)
                       view.contentDOM.style.pointerEvents = "none";
                   else
                       window.clearTimeout(this.chromeScrollHack);
                   this.chromeScrollHack = setTimeout(() => {
                       this.chromeScrollHack = -1;
                       view.contentDOM.style.pointerEvents = "";
                   }, 100);
               }, { passive: true });
           }
           this.notifiedFocused = view.hasFocus;
           // On Safari adding an input event handler somehow prevents an
           // issue where the composition vanishes when you press enter.
           if (browser.safari)
               view.contentDOM.addEventListener("input", () => null);
       }
       setSelectionOrigin(origin) {
           this.lastSelectionOrigin = origin;
           this.lastSelectionTime = Date.now();
       }
       ensureHandlers(view, plugins) {
           var _a;
           let handlers;
           this.customHandlers = [];
           for (let plugin of plugins)
               if (handlers = (_a = plugin.update(view).spec) === null || _a === void 0 ? void 0 : _a.domEventHandlers) {
                   this.customHandlers.push({ plugin: plugin.value, handlers });
                   for (let type in handlers)
                       if (this.registeredEvents.indexOf(type) < 0 && type != "scroll") {
                           this.registeredEvents.push(type);
                           view.contentDOM.addEventListener(type, (event) => {
                               if (!eventBelongsToEditor(view, event))
                                   return;
                               if (this.runCustomHandlers(type, view, event))
                                   event.preventDefault();
                           });
                       }
               }
       }
       runCustomHandlers(type, view, event) {
           for (let set of this.customHandlers) {
               let handler = set.handlers[type];
               if (handler) {
                   try {
                       if (handler.call(set.plugin, event, view) || event.defaultPrevented)
                           return true;
                   }
                   catch (e) {
                       logException(view.state, e);
                   }
               }
           }
           return false;
       }
       runScrollHandlers(view, event) {
           this.lastScrollTop = view.scrollDOM.scrollTop;
           this.lastScrollLeft = view.scrollDOM.scrollLeft;
           for (let set of this.customHandlers) {
               let handler = set.handlers.scroll;
               if (handler) {
                   try {
                       handler.call(set.plugin, event, view);
                   }
                   catch (e) {
                       logException(view.state, e);
                   }
               }
           }
       }
       keydown(view, event) {
           // Must always run, even if a custom handler handled the event
           this.lastKeyCode = event.keyCode;
           this.lastKeyTime = Date.now();
           if (event.keyCode == 9 && Date.now() < this.lastEscPress + 2000)
               return true;
           // Chrome for Android usually doesn't fire proper key events, but
           // occasionally does, usually surrounded by a bunch of complicated
           // composition changes. When an enter or backspace key event is
           // seen, hold off on handling DOM events for a bit, and then
           // dispatch it.
           if (browser.android && browser.chrome && !event.synthetic &&
               (event.keyCode == 13 || event.keyCode == 8)) {
               view.observer.delayAndroidKey(event.key, event.keyCode);
               return true;
           }
           // Prevent the default behavior of Enter on iOS makes the
           // virtual keyboard get stuck in the wrong (lowercase)
           // state. So we let it go through, and then, in
           // applyDOMChange, notify key handlers of it and reset to
           // the state they produce.
           let pending;
           if (browser.ios && !event.synthetic && !event.altKey && !event.metaKey &&
               ((pending = PendingKeys.find(key => key.keyCode == event.keyCode)) && !event.ctrlKey ||
                   EmacsyPendingKeys.indexOf(event.key) > -1 && event.ctrlKey && !event.shiftKey)) {
               this.pendingIOSKey = pending || event;
               setTimeout(() => this.flushIOSKey(view), 250);
               return true;
           }
           return false;
       }
       flushIOSKey(view) {
           let key = this.pendingIOSKey;
           if (!key)
               return false;
           this.pendingIOSKey = undefined;
           return dispatchKey(view.contentDOM, key.key, key.keyCode);
       }
       ignoreDuringComposition(event) {
           if (!/^key/.test(event.type))
               return false;
           if (this.composing > 0)
               return true;
           // See https://www.stum.de/2016/06/24/handling-ime-events-in-javascript/.
           // On some input method editors (IMEs), the Enter key is used to
           // confirm character selection. On Safari, when Enter is pressed,
           // compositionend and keydown events are sometimes emitted in the
           // wrong order. The key event should still be ignored, even when
           // it happens after the compositionend event.
           if (browser.safari && !browser.ios && Date.now() - this.compositionEndedAt < 100) {
               this.compositionEndedAt = 0;
               return true;
           }
           return false;
       }
       mustFlushObserver(event) {
           return event.type == "keydown" && event.keyCode != 229;
       }
       startMouseSelection(mouseSelection) {
           if (this.mouseSelection)
               this.mouseSelection.destroy();
           this.mouseSelection = mouseSelection;
       }
       update(update) {
           if (this.mouseSelection)
               this.mouseSelection.update(update);
           if (update.transactions.length)
               this.lastKeyCode = this.lastSelectionTime = 0;
       }
       destroy() {
           if (this.mouseSelection)
               this.mouseSelection.destroy();
       }
   }
   const PendingKeys = [
       { key: "Backspace", keyCode: 8, inputType: "deleteContentBackward" },
       { key: "Enter", keyCode: 13, inputType: "insertParagraph" },
       { key: "Delete", keyCode: 46, inputType: "deleteContentForward" }
   ];
   const EmacsyPendingKeys = "dthko";
   // Key codes for modifier keys
   const modifierCodes = [16, 17, 18, 20, 91, 92, 224, 225];
   class MouseSelection {
       constructor(view, startEvent, style, mustSelect) {
           this.view = view;
           this.style = style;
           this.mustSelect = mustSelect;
           this.lastEvent = startEvent;
           let doc = view.contentDOM.ownerDocument;
           doc.addEventListener("mousemove", this.move = this.move.bind(this));
           doc.addEventListener("mouseup", this.up = this.up.bind(this));
           this.extend = startEvent.shiftKey;
           this.multiple = view.state.facet(EditorState.allowMultipleSelections) && addsSelectionRange(view, startEvent);
           this.dragMove = dragMovesSelection(view, startEvent);
           this.dragging = isInPrimarySelection(view, startEvent) && getClickType(startEvent) == 1 ? null : false;
           // When clicking outside of the selection, immediately apply the
           // effect of starting the selection
           if (this.dragging === false) {
               startEvent.preventDefault();
               this.select(startEvent);
           }
       }
       move(event) {
           if (event.buttons == 0)
               return this.destroy();
           if (this.dragging !== false)
               return;
           this.select(this.lastEvent = event);
       }
       up(event) {
           if (this.dragging == null)
               this.select(this.lastEvent);
           if (!this.dragging)
               event.preventDefault();
           this.destroy();
       }
       destroy() {
           let doc = this.view.contentDOM.ownerDocument;
           doc.removeEventListener("mousemove", this.move);
           doc.removeEventListener("mouseup", this.up);
           this.view.inputState.mouseSelection = null;
       }
       select(event) {
           let selection = this.style.get(event, this.extend, this.multiple);
           if (this.mustSelect || !selection.eq(this.view.state.selection) ||
               selection.main.assoc != this.view.state.selection.main.assoc)
               this.view.dispatch({
                   selection,
                   userEvent: "select.pointer",
                   scrollIntoView: true
               });
           this.mustSelect = false;
       }
       update(update) {
           if (update.docChanged && this.dragging)
               this.dragging = this.dragging.map(update.changes);
           if (this.style.update(update))
               setTimeout(() => this.select(this.lastEvent), 20);
       }
   }
   function addsSelectionRange(view, event) {
       let facet = view.state.facet(clickAddsSelectionRange);
       return facet.length ? facet[0](event) : browser.mac ? event.metaKey : event.ctrlKey;
   }
   function dragMovesSelection(view, event) {
       let facet = view.state.facet(dragMovesSelection$1);
       return facet.length ? facet[0](event) : browser.mac ? !event.altKey : !event.ctrlKey;
   }
   function isInPrimarySelection(view, event) {
       let { main } = view.state.selection;
       if (main.empty)
           return false;
       // On boundary clicks, check whether the coordinates are inside the
       // selection's client rectangles
       let sel = getSelection(view.root);
       if (!sel || sel.rangeCount == 0)
           return true;
       let rects = sel.getRangeAt(0).getClientRects();
       for (let i = 0; i < rects.length; i++) {
           let rect = rects[i];
           if (rect.left <= event.clientX && rect.right >= event.clientX &&
               rect.top <= event.clientY && rect.bottom >= event.clientY)
               return true;
       }
       return false;
   }
   function eventBelongsToEditor(view, event) {
       if (!event.bubbles)
           return true;
       if (event.defaultPrevented)
           return false;
       for (let node = event.target, cView; node != view.contentDOM; node = node.parentNode)
           if (!node || node.nodeType == 11 || ((cView = ContentView.get(node)) && cView.ignoreEvent(event)))
               return false;
       return true;
   }
   const handlers = /*@__PURE__*/Object.create(null);
   const handlerOptions = /*@__PURE__*/Object.create(null);
   // This is very crude, but unfortunately both these browsers _pretend_
   // that they have a clipboard API—all the objects and methods are
   // there, they just don't work, and they are hard to test.
   const brokenClipboardAPI = (browser.ie && browser.ie_version < 15) ||
       (browser.ios && browser.webkit_version < 604);
   function capturePaste(view) {
       let parent = view.dom.parentNode;
       if (!parent)
           return;
       let target = parent.appendChild(document.createElement("textarea"));
       target.style.cssText = "position: fixed; left: -10000px; top: 10px";
       target.focus();
       setTimeout(() => {
           view.focus();
           target.remove();
           doPaste(view, target.value);
       }, 50);
   }
   function doPaste(view, input) {
       let { state } = view, changes, i = 1, text = state.toText(input);
       let byLine = text.lines == state.selection.ranges.length;
       let linewise = lastLinewiseCopy != null && state.selection.ranges.every(r => r.empty) && lastLinewiseCopy == text.toString();
       if (linewise) {
           let lastLine = -1;
           changes = state.changeByRange(range => {
               let line = state.doc.lineAt(range.from);
               if (line.from == lastLine)
                   return { range };
               lastLine = line.from;
               let insert = state.toText((byLine ? text.line(i++).text : input) + state.lineBreak);
               return { changes: { from: line.from, insert },
                   range: EditorSelection.cursor(range.from + insert.length) };
           });
       }
       else if (byLine) {
           changes = state.changeByRange(range => {
               let line = text.line(i++);
               return { changes: { from: range.from, to: range.to, insert: line.text },
                   range: EditorSelection.cursor(range.from + line.length) };
           });
       }
       else {
           changes = state.replaceSelection(text);
       }
       view.dispatch(changes, {
           userEvent: "input.paste",
           scrollIntoView: true
       });
   }
   handlers.keydown = (view, event) => {
       view.inputState.setSelectionOrigin("select");
       if (event.keyCode == 27)
           view.inputState.lastEscPress = Date.now();
       else if (modifierCodes.indexOf(event.keyCode) < 0)
           view.inputState.lastEscPress = 0;
   };
   handlers.touchstart = (view, e) => {
       view.inputState.lastTouchTime = Date.now();
       view.inputState.setSelectionOrigin("select.pointer");
   };
   handlers.touchmove = view => {
       view.inputState.setSelectionOrigin("select.pointer");
   };
   handlerOptions.touchstart = handlerOptions.touchmove = { passive: true };
   handlers.mousedown = (view, event) => {
       view.observer.flush();
       if (view.inputState.lastTouchTime > Date.now() - 2000 && getClickType(event) == 1)
           return; // Ignore touch interaction
       let style = null;
       for (let makeStyle of view.state.facet(mouseSelectionStyle)) {
           style = makeStyle(view, event);
           if (style)
               break;
       }
       if (!style && event.button == 0)
           style = basicMouseSelection(view, event);
       if (style) {
           let mustFocus = view.root.activeElement != view.contentDOM;
           if (mustFocus)
               view.observer.ignore(() => focusPreventScroll(view.contentDOM));
           view.inputState.startMouseSelection(new MouseSelection(view, event, style, mustFocus));
       }
   };
   function rangeForClick(view, pos, bias, type) {
       if (type == 1) { // Single click
           return EditorSelection.cursor(pos, bias);
       }
       else if (type == 2) { // Double click
           return groupAt(view.state, pos, bias);
       }
       else { // Triple click
           let visual = LineView.find(view.docView, pos), line = view.state.doc.lineAt(visual ? visual.posAtEnd : pos);
           let from = visual ? visual.posAtStart : line.from, to = visual ? visual.posAtEnd : line.to;
           if (to < view.state.doc.length && to == line.to)
               to++;
           return EditorSelection.range(from, to);
       }
   }
   let insideY = (y, rect) => y >= rect.top && y <= rect.bottom;
   let inside = (x, y, rect) => insideY(y, rect) && x >= rect.left && x <= rect.right;
   // Try to determine, for the given coordinates, associated with the
   // given position, whether they are related to the element before or
   // the element after the position.
   function findPositionSide(view, pos, x, y) {
       let line = LineView.find(view.docView, pos);
       if (!line)
           return 1;
       let off = pos - line.posAtStart;
       // Line boundaries point into the line
       if (off == 0)
           return 1;
       if (off == line.length)
           return -1;
       // Positions on top of an element point at that element
       let before = line.coordsAt(off, -1);
       if (before && inside(x, y, before))
           return -1;
       let after = line.coordsAt(off, 1);
       if (after && inside(x, y, after))
           return 1;
       // This is probably a line wrap point. Pick before if the point is
       // beside it.
       return before && insideY(y, before) ? -1 : 1;
   }
   function queryPos(view, event) {
       let pos = view.posAtCoords({ x: event.clientX, y: event.clientY }, false);
       return { pos, bias: findPositionSide(view, pos, event.clientX, event.clientY) };
   }
   const BadMouseDetail = browser.ie && browser.ie_version <= 11;
   let lastMouseDown = null, lastMouseDownCount = 0, lastMouseDownTime = 0;
   function getClickType(event) {
       if (!BadMouseDetail)
           return event.detail;
       let last = lastMouseDown, lastTime = lastMouseDownTime;
       lastMouseDown = event;
       lastMouseDownTime = Date.now();
       return lastMouseDownCount = !last || (lastTime > Date.now() - 400 && Math.abs(last.clientX - event.clientX) < 2 &&
           Math.abs(last.clientY - event.clientY) < 2) ? (lastMouseDownCount + 1) % 3 : 1;
   }
   function basicMouseSelection(view, event) {
       let start = queryPos(view, event), type = getClickType(event);
       let startSel = view.state.selection;
       let last = start, lastEvent = event;
       return {
           update(update) {
               if (update.docChanged) {
                   start.pos = update.changes.mapPos(start.pos);
                   startSel = startSel.map(update.changes);
                   lastEvent = null;
               }
           },
           get(event, extend, multiple) {
               let cur;
               if (lastEvent && event.clientX == lastEvent.clientX && event.clientY == lastEvent.clientY)
                   cur = last;
               else {
                   cur = last = queryPos(view, event);
                   lastEvent = event;
               }
               let range = rangeForClick(view, cur.pos, cur.bias, type);
               if (start.pos != cur.pos && !extend) {
                   let startRange = rangeForClick(view, start.pos, start.bias, type);
                   let from = Math.min(startRange.from, range.from), to = Math.max(startRange.to, range.to);
                   range = from < range.from ? EditorSelection.range(from, to) : EditorSelection.range(to, from);
               }
               if (extend)
                   return startSel.replaceRange(startSel.main.extend(range.from, range.to));
               else if (multiple && startSel.ranges.length > 1 && startSel.ranges.some(r => r.eq(range)))
                   return removeRange(startSel, range);
               else if (multiple)
                   return startSel.addRange(range);
               else
                   return EditorSelection.create([range]);
           }
       };
   }
   function removeRange(sel, range) {
       for (let i = 0;; i++) {
           if (sel.ranges[i].eq(range))
               return EditorSelection.create(sel.ranges.slice(0, i).concat(sel.ranges.slice(i + 1)), sel.mainIndex == i ? 0 : sel.mainIndex - (sel.mainIndex > i ? 1 : 0));
       }
   }
   handlers.dragstart = (view, event) => {
       let { selection: { main } } = view.state;
       let { mouseSelection } = view.inputState;
       if (mouseSelection)
           mouseSelection.dragging = main;
       if (event.dataTransfer) {
           event.dataTransfer.setData("Text", view.state.sliceDoc(main.from, main.to));
           event.dataTransfer.effectAllowed = "copyMove";
       }
   };
   function dropText(view, event, text, direct) {
       if (!text)
           return;
       let dropPos = view.posAtCoords({ x: event.clientX, y: event.clientY }, false);
       event.preventDefault();
       let { mouseSelection } = view.inputState;
       let del = direct && mouseSelection && mouseSelection.dragging && mouseSelection.dragMove ?
           { from: mouseSelection.dragging.from, to: mouseSelection.dragging.to } : null;
       let ins = { from: dropPos, insert: text };
       let changes = view.state.changes(del ? [del, ins] : ins);
       view.focus();
       view.dispatch({
           changes,
           selection: { anchor: changes.mapPos(dropPos, -1), head: changes.mapPos(dropPos, 1) },
           userEvent: del ? "move.drop" : "input.drop"
       });
   }
   handlers.drop = (view, event) => {
       if (!event.dataTransfer)
           return;
       if (view.state.readOnly)
           return event.preventDefault();
       let files = event.dataTransfer.files;
       if (files && files.length) { // For a file drop, read the file's text.
           event.preventDefault();
           let text = Array(files.length), read = 0;
           let finishFile = () => {
               if (++read == files.length)
                   dropText(view, event, text.filter(s => s != null).join(view.state.lineBreak), false);
           };
           for (let i = 0; i < files.length; i++) {
               let reader = new FileReader;
               reader.onerror = finishFile;
               reader.onload = () => {
                   if (!/[\x00-\x08\x0e-\x1f]{2}/.test(reader.result))
                       text[i] = reader.result;
                   finishFile();
               };
               reader.readAsText(files[i]);
           }
       }
       else {
           dropText(view, event, event.dataTransfer.getData("Text"), true);
       }
   };
   handlers.paste = (view, event) => {
       if (view.state.readOnly)
           return event.preventDefault();
       view.observer.flush();
       let data = brokenClipboardAPI ? null : event.clipboardData;
       if (data) {
           doPaste(view, data.getData("text/plain"));
           event.preventDefault();
       }
       else {
           capturePaste(view);
       }
   };
   function captureCopy(view, text) {
       // The extra wrapper is somehow necessary on IE/Edge to prevent the
       // content from being mangled when it is put onto the clipboard
       let parent = view.dom.parentNode;
       if (!parent)
           return;
       let target = parent.appendChild(document.createElement("textarea"));
       target.style.cssText = "position: fixed; left: -10000px; top: 10px";
       target.value = text;
       target.focus();
       target.selectionEnd = text.length;
       target.selectionStart = 0;
       setTimeout(() => {
           target.remove();
           view.focus();
       }, 50);
   }
   function copiedRange(state) {
       let content = [], ranges = [], linewise = false;
       for (let range of state.selection.ranges)
           if (!range.empty) {
               content.push(state.sliceDoc(range.from, range.to));
               ranges.push(range);
           }
       if (!content.length) {
           // Nothing selected, do a line-wise copy
           let upto = -1;
           for (let { from } of state.selection.ranges) {
               let line = state.doc.lineAt(from);
               if (line.number > upto) {
                   content.push(line.text);
                   ranges.push({ from: line.from, to: Math.min(state.doc.length, line.to + 1) });
               }
               upto = line.number;
           }
           linewise = true;
       }
       return { text: content.join(state.lineBreak), ranges, linewise };
   }
   let lastLinewiseCopy = null;
   handlers.copy = handlers.cut = (view, event) => {
       let { text, ranges, linewise } = copiedRange(view.state);
       if (!text && !linewise)
           return;
       lastLinewiseCopy = linewise ? text : null;
       let data = brokenClipboardAPI ? null : event.clipboardData;
       if (data) {
           event.preventDefault();
           data.clearData();
           data.setData("text/plain", text);
       }
       else {
           captureCopy(view, text);
       }
       if (event.type == "cut" && !view.state.readOnly)
           view.dispatch({
               changes: ranges,
               scrollIntoView: true,
               userEvent: "delete.cut"
           });
   };
   function updateForFocusChange(view) {
       setTimeout(() => {
           if (view.hasFocus != view.inputState.notifiedFocused)
               view.update([]);
       }, 10);
   }
   handlers.focus = view => {
       view.inputState.lastFocusTime = Date.now();
       // When focusing reset the scroll position, move it back to where it was
       if (!view.scrollDOM.scrollTop && (view.inputState.lastScrollTop || view.inputState.lastScrollLeft)) {
           view.scrollDOM.scrollTop = view.inputState.lastScrollTop;
           view.scrollDOM.scrollLeft = view.inputState.lastScrollLeft;
       }
       updateForFocusChange(view);
   };
   handlers.blur = view => {
       view.observer.clearSelectionRange();
       updateForFocusChange(view);
   };
   handlers.compositionstart = handlers.compositionupdate = view => {
       if (view.inputState.compositionFirstChange == null)
           view.inputState.compositionFirstChange = true;
       if (view.inputState.composing < 0) {
           // FIXME possibly set a timeout to clear it again on Android
           view.inputState.composing = 0;
       }
   };
   handlers.compositionend = view => {
       view.inputState.composing = -1;
       view.inputState.compositionEndedAt = Date.now();
       view.inputState.compositionFirstChange = null;
       if (browser.chrome && browser.android)
           view.observer.flushSoon();
       setTimeout(() => {
           // Force the composition state to be cleared if it hasn't already been
           if (view.inputState.composing < 0 && view.docView.compositionDeco.size)
               view.update([]);
       }, 50);
   };
   handlers.contextmenu = view => {
       view.inputState.lastContextMenu = Date.now();
   };
   handlers.beforeinput = (view, event) => {
       var _a;
       // Because Chrome Android doesn't fire useful key events, use
       // beforeinput to detect backspace (and possibly enter and delete,
       // but those usually don't even seem to fire beforeinput events at
       // the moment) and fake a key event for it.
       //
       // (preventDefault on beforeinput, though supported in the spec,
       // seems to do nothing at all on Chrome).
       let pending;
       if (browser.chrome && browser.android && (pending = PendingKeys.find(key => key.inputType == event.inputType))) {
           view.observer.delayAndroidKey(pending.key, pending.keyCode);
           if (pending.key == "Backspace" || pending.key == "Delete") {
               let startViewHeight = ((_a = window.visualViewport) === null || _a === void 0 ? void 0 : _a.height) || 0;
               setTimeout(() => {
                   var _a;
                   // Backspacing near uneditable nodes on Chrome Android sometimes
                   // closes the virtual keyboard. This tries to crudely detect
                   // that and refocus to get it back.
                   if ((((_a = window.visualViewport) === null || _a === void 0 ? void 0 : _a.height) || 0) > startViewHeight + 10 && view.hasFocus) {
                       view.contentDOM.blur();
                       view.focus();
                   }
               }, 100);
           }
       }
   };

   const wrappingWhiteSpace = ["pre-wrap", "normal", "pre-line", "break-spaces"];
   class HeightOracle {
       constructor() {
           this.doc = Text.empty;
           this.lineWrapping = false;
           this.heightSamples = {};
           this.lineHeight = 14;
           this.charWidth = 7;
           this.lineLength = 30;
           // Used to track, during updateHeight, if any actual heights changed
           this.heightChanged = false;
       }
       heightForGap(from, to) {
           let lines = this.doc.lineAt(to).number - this.doc.lineAt(from).number + 1;
           if (this.lineWrapping)
               lines += Math.ceil(((to - from) - (lines * this.lineLength * 0.5)) / this.lineLength);
           return this.lineHeight * lines;
       }
       heightForLine(length) {
           if (!this.lineWrapping)
               return this.lineHeight;
           let lines = 1 + Math.max(0, Math.ceil((length - this.lineLength) / (this.lineLength - 5)));
           return lines * this.lineHeight;
       }
       setDoc(doc) { this.doc = doc; return this; }
       mustRefreshForWrapping(whiteSpace) {
           return (wrappingWhiteSpace.indexOf(whiteSpace) > -1) != this.lineWrapping;
       }
       mustRefreshForHeights(lineHeights) {
           let newHeight = false;
           for (let i = 0; i < lineHeights.length; i++) {
               let h = lineHeights[i];
               if (h < 0) {
                   i++;
               }
               else if (!this.heightSamples[Math.floor(h * 10)]) { // Round to .1 pixels
                   newHeight = true;
                   this.heightSamples[Math.floor(h * 10)] = true;
               }
           }
           return newHeight;
       }
       refresh(whiteSpace, lineHeight, charWidth, lineLength, knownHeights) {
           let lineWrapping = wrappingWhiteSpace.indexOf(whiteSpace) > -1;
           let changed = Math.round(lineHeight) != Math.round(this.lineHeight) || this.lineWrapping != lineWrapping;
           this.lineWrapping = lineWrapping;
           this.lineHeight = lineHeight;
           this.charWidth = charWidth;
           this.lineLength = lineLength;
           if (changed) {
               this.heightSamples = {};
               for (let i = 0; i < knownHeights.length; i++) {
                   let h = knownHeights[i];
                   if (h < 0)
                       i++;
                   else
                       this.heightSamples[Math.floor(h * 10)] = true;
               }
           }
           return changed;
       }
   }
   // This object is used by `updateHeight` to make DOM measurements
   // arrive at the right nides. The `heights` array is a sequence of
   // block heights, starting from position `from`.
   class MeasuredHeights {
       constructor(from, heights) {
           this.from = from;
           this.heights = heights;
           this.index = 0;
       }
       get more() { return this.index < this.heights.length; }
   }
   /**
   Record used to represent information about a block-level element
   in the editor view.
   */
   class BlockInfo {
       /**
       @internal
       */
       constructor(
       /**
       The start of the element in the document.
       */
       from, 
       /**
       The length of the element.
       */
       length, 
       /**
       The top position of the element (relative to the top of the
       document).
       */
       top, 
       /**
       Its height.
       */
       height, 
       /**
       The type of element this is. When querying lines, this may be
       an array of all the blocks that make up the line.
       */
       type) {
           this.from = from;
           this.length = length;
           this.top = top;
           this.height = height;
           this.type = type;
       }
       /**
       The end of the element as a document position.
       */
       get to() { return this.from + this.length; }
       /**
       The bottom position of the element.
       */
       get bottom() { return this.top + this.height; }
       /**
       @internal
       */
       join(other) {
           let detail = (Array.isArray(this.type) ? this.type : [this])
               .concat(Array.isArray(other.type) ? other.type : [other]);
           return new BlockInfo(this.from, this.length + other.length, this.top, this.height + other.height, detail);
       }
   }
   var QueryType = /*@__PURE__*/(function (QueryType) {
       QueryType[QueryType["ByPos"] = 0] = "ByPos";
       QueryType[QueryType["ByHeight"] = 1] = "ByHeight";
       QueryType[QueryType["ByPosNoHeight"] = 2] = "ByPosNoHeight";
   return QueryType})(QueryType || (QueryType = {}));
   const Epsilon = 1e-3;
   class HeightMap {
       constructor(length, // The number of characters covered
       height, // Height of this part of the document
       flags = 2 /* Flag.Outdated */) {
           this.length = length;
           this.height = height;
           this.flags = flags;
       }
       get outdated() { return (this.flags & 2 /* Flag.Outdated */) > 0; }
       set outdated(value) { this.flags = (value ? 2 /* Flag.Outdated */ : 0) | (this.flags & ~2 /* Flag.Outdated */); }
       setHeight(oracle, height) {
           if (this.height != height) {
               if (Math.abs(this.height - height) > Epsilon)
                   oracle.heightChanged = true;
               this.height = height;
           }
       }
       // Base case is to replace a leaf node, which simply builds a tree
       // from the new nodes and returns that (HeightMapBranch and
       // HeightMapGap override this to actually use from/to)
       replace(_from, _to, nodes) {
           return HeightMap.of(nodes);
       }
       // Again, these are base cases, and are overridden for branch and gap nodes.
       decomposeLeft(_to, result) { result.push(this); }
       decomposeRight(_from, result) { result.push(this); }
       applyChanges(decorations, oldDoc, oracle, changes) {
           let me = this;
           for (let i = changes.length - 1; i >= 0; i--) {
               let { fromA, toA, fromB, toB } = changes[i];
               let start = me.lineAt(fromA, QueryType.ByPosNoHeight, oldDoc, 0, 0);
               let end = start.to >= toA ? start : me.lineAt(toA, QueryType.ByPosNoHeight, oldDoc, 0, 0);
               toB += end.to - toA;
               toA = end.to;
               while (i > 0 && start.from <= changes[i - 1].toA) {
                   fromA = changes[i - 1].fromA;
                   fromB = changes[i - 1].fromB;
                   i--;
                   if (fromA < start.from)
                       start = me.lineAt(fromA, QueryType.ByPosNoHeight, oldDoc, 0, 0);
               }
               fromB += start.from - fromA;
               fromA = start.from;
               let nodes = NodeBuilder.build(oracle, decorations, fromB, toB);
               me = me.replace(fromA, toA, nodes);
           }
           return me.updateHeight(oracle, 0);
       }
       static empty() { return new HeightMapText(0, 0); }
       // nodes uses null values to indicate the position of line breaks.
       // There are never line breaks at the start or end of the array, or
       // two line breaks next to each other, and the array isn't allowed
       // to be empty (same restrictions as return value from the builder).
       static of(nodes) {
           if (nodes.length == 1)
               return nodes[0];
           let i = 0, j = nodes.length, before = 0, after = 0;
           for (;;) {
               if (i == j) {
                   if (before > after * 2) {
                       let split = nodes[i - 1];
                       if (split.break)
                           nodes.splice(--i, 1, split.left, null, split.right);
                       else
                           nodes.splice(--i, 1, split.left, split.right);
                       j += 1 + split.break;
                       before -= split.size;
                   }
                   else if (after > before * 2) {
                       let split = nodes[j];
                       if (split.break)
                           nodes.splice(j, 1, split.left, null, split.right);
                       else
                           nodes.splice(j, 1, split.left, split.right);
                       j += 2 + split.break;
                       after -= split.size;
                   }
                   else {
                       break;
                   }
               }
               else if (before < after) {
                   let next = nodes[i++];
                   if (next)
                       before += next.size;
               }
               else {
                   let next = nodes[--j];
                   if (next)
                       after += next.size;
               }
           }
           let brk = 0;
           if (nodes[i - 1] == null) {
               brk = 1;
               i--;
           }
           else if (nodes[i] == null) {
               brk = 1;
               j++;
           }
           return new HeightMapBranch(HeightMap.of(nodes.slice(0, i)), brk, HeightMap.of(nodes.slice(j)));
       }
   }
   HeightMap.prototype.size = 1;
   class HeightMapBlock extends HeightMap {
       constructor(length, height, type) {
           super(length, height);
           this.type = type;
       }
       blockAt(_height, _doc, top, offset) {
           return new BlockInfo(offset, this.length, top, this.height, this.type);
       }
       lineAt(_value, _type, doc, top, offset) {
           return this.blockAt(0, doc, top, offset);
       }
       forEachLine(from, to, doc, top, offset, f) {
           if (from <= offset + this.length && to >= offset)
               f(this.blockAt(0, doc, top, offset));
       }
       updateHeight(oracle, offset = 0, _force = false, measured) {
           if (measured && measured.from <= offset && measured.more)
               this.setHeight(oracle, measured.heights[measured.index++]);
           this.outdated = false;
           return this;
       }
       toString() { return `block(${this.length})`; }
   }
   class HeightMapText extends HeightMapBlock {
       constructor(length, height) {
           super(length, height, BlockType.Text);
           this.collapsed = 0; // Amount of collapsed content in the line
           this.widgetHeight = 0; // Maximum inline widget height
       }
       replace(_from, _to, nodes) {
           let node = nodes[0];
           if (nodes.length == 1 && (node instanceof HeightMapText || node instanceof HeightMapGap && (node.flags & 4 /* Flag.SingleLine */)) &&
               Math.abs(this.length - node.length) < 10) {
               if (node instanceof HeightMapGap)
                   node = new HeightMapText(node.length, this.height);
               else
                   node.height = this.height;
               if (!this.outdated)
                   node.outdated = false;
               return node;
           }
           else {
               return HeightMap.of(nodes);
           }
       }
       updateHeight(oracle, offset = 0, force = false, measured) {
           if (measured && measured.from <= offset && measured.more)
               this.setHeight(oracle, measured.heights[measured.index++]);
           else if (force || this.outdated)
               this.setHeight(oracle, Math.max(this.widgetHeight, oracle.heightForLine(this.length - this.collapsed)));
           this.outdated = false;
           return this;
       }
       toString() {
           return `line(${this.length}${this.collapsed ? -this.collapsed : ""}${this.widgetHeight ? ":" + this.widgetHeight : ""})`;
       }
   }
   class HeightMapGap extends HeightMap {
       constructor(length) { super(length, 0); }
       lines(doc, offset) {
           let firstLine = doc.lineAt(offset).number, lastLine = doc.lineAt(offset + this.length).number;
           return { firstLine, lastLine, lineHeight: this.height / (lastLine - firstLine + 1) };
       }
       blockAt(height, doc, top, offset) {
           let { firstLine, lastLine, lineHeight } = this.lines(doc, offset);
           let line = Math.max(0, Math.min(lastLine - firstLine, Math.floor((height - top) / lineHeight)));
           let { from, length } = doc.line(firstLine + line);
           return new BlockInfo(from, length, top + lineHeight * line, lineHeight, BlockType.Text);
       }
       lineAt(value, type, doc, top, offset) {
           if (type == QueryType.ByHeight)
               return this.blockAt(value, doc, top, offset);
           if (type == QueryType.ByPosNoHeight) {
               let { from, to } = doc.lineAt(value);
               return new BlockInfo(from, to - from, 0, 0, BlockType.Text);
           }
           let { firstLine, lineHeight } = this.lines(doc, offset);
           let { from, length, number } = doc.lineAt(value);
           return new BlockInfo(from, length, top + lineHeight * (number - firstLine), lineHeight, BlockType.Text);
       }
       forEachLine(from, to, doc, top, offset, f) {
           let { firstLine, lineHeight } = this.lines(doc, offset);
           for (let pos = Math.max(from, offset), end = Math.min(offset + this.length, to); pos <= end;) {
               let line = doc.lineAt(pos);
               if (pos == from)
                   top += lineHeight * (line.number - firstLine);
               f(new BlockInfo(line.from, line.length, top, lineHeight, BlockType.Text));
               top += lineHeight;
               pos = line.to + 1;
           }
       }
       replace(from, to, nodes) {
           let after = this.length - to;
           if (after > 0) {
               let last = nodes[nodes.length - 1];
               if (last instanceof HeightMapGap)
                   nodes[nodes.length - 1] = new HeightMapGap(last.length + after);
               else
                   nodes.push(null, new HeightMapGap(after - 1));
           }
           if (from > 0) {
               let first = nodes[0];
               if (first instanceof HeightMapGap)
                   nodes[0] = new HeightMapGap(from + first.length);
               else
                   nodes.unshift(new HeightMapGap(from - 1), null);
           }
           return HeightMap.of(nodes);
       }
       decomposeLeft(to, result) {
           result.push(new HeightMapGap(to - 1), null);
       }
       decomposeRight(from, result) {
           result.push(null, new HeightMapGap(this.length - from - 1));
       }
       updateHeight(oracle, offset = 0, force = false, measured) {
           let end = offset + this.length;
           if (measured && measured.from <= offset + this.length && measured.more) {
               // Fill in part of this gap with measured lines. We know there
               // can't be widgets or collapsed ranges in those lines, because
               // they would already have been added to the heightmap (gaps
               // only contain plain text).
               let nodes = [], pos = Math.max(offset, measured.from), singleHeight = -1;
               let wasChanged = oracle.heightChanged;
               if (measured.from > offset)
                   nodes.push(new HeightMapGap(measured.from - offset - 1).updateHeight(oracle, offset));
               while (pos <= end && measured.more) {
                   let len = oracle.doc.lineAt(pos).length;
                   if (nodes.length)
                       nodes.push(null);
                   let height = measured.heights[measured.index++];
                   if (singleHeight == -1)
                       singleHeight = height;
                   else if (Math.abs(height - singleHeight) >= Epsilon)
                       singleHeight = -2;
                   let line = new HeightMapText(len, height);
                   line.outdated = false;
                   nodes.push(line);
                   pos += len + 1;
               }
               if (pos <= end)
                   nodes.push(null, new HeightMapGap(end - pos).updateHeight(oracle, pos));
               let result = HeightMap.of(nodes);
               oracle.heightChanged = wasChanged || singleHeight < 0 || Math.abs(result.height - this.height) >= Epsilon ||
                   Math.abs(singleHeight - this.lines(oracle.doc, offset).lineHeight) >= Epsilon;
               return result;
           }
           else if (force || this.outdated) {
               this.setHeight(oracle, oracle.heightForGap(offset, offset + this.length));
               this.outdated = false;
           }
           return this;
       }
       toString() { return `gap(${this.length})`; }
   }
   class HeightMapBranch extends HeightMap {
       constructor(left, brk, right) {
           super(left.length + brk + right.length, left.height + right.height, brk | (left.outdated || right.outdated ? 2 /* Flag.Outdated */ : 0));
           this.left = left;
           this.right = right;
           this.size = left.size + right.size;
       }
       get break() { return this.flags & 1 /* Flag.Break */; }
       blockAt(height, doc, top, offset) {
           let mid = top + this.left.height;
           return height < mid ? this.left.blockAt(height, doc, top, offset)
               : this.right.blockAt(height, doc, mid, offset + this.left.length + this.break);
       }
       lineAt(value, type, doc, top, offset) {
           let rightTop = top + this.left.height, rightOffset = offset + this.left.length + this.break;
           let left = type == QueryType.ByHeight ? value < rightTop : value < rightOffset;
           let base = left ? this.left.lineAt(value, type, doc, top, offset)
               : this.right.lineAt(value, type, doc, rightTop, rightOffset);
           if (this.break || (left ? base.to < rightOffset : base.from > rightOffset))
               return base;
           let subQuery = type == QueryType.ByPosNoHeight ? QueryType.ByPosNoHeight : QueryType.ByPos;
           if (left)
               return base.join(this.right.lineAt(rightOffset, subQuery, doc, rightTop, rightOffset));
           else
               return this.left.lineAt(rightOffset, subQuery, doc, top, offset).join(base);
       }
       forEachLine(from, to, doc, top, offset, f) {
           let rightTop = top + this.left.height, rightOffset = offset + this.left.length + this.break;
           if (this.break) {
               if (from < rightOffset)
                   this.left.forEachLine(from, to, doc, top, offset, f);
               if (to >= rightOffset)
                   this.right.forEachLine(from, to, doc, rightTop, rightOffset, f);
           }
           else {
               let mid = this.lineAt(rightOffset, QueryType.ByPos, doc, top, offset);
               if (from < mid.from)
                   this.left.forEachLine(from, mid.from - 1, doc, top, offset, f);
               if (mid.to >= from && mid.from <= to)
                   f(mid);
               if (to > mid.to)
                   this.right.forEachLine(mid.to + 1, to, doc, rightTop, rightOffset, f);
           }
       }
       replace(from, to, nodes) {
           let rightStart = this.left.length + this.break;
           if (to < rightStart)
               return this.balanced(this.left.replace(from, to, nodes), this.right);
           if (from > this.left.length)
               return this.balanced(this.left, this.right.replace(from - rightStart, to - rightStart, nodes));
           let result = [];
           if (from > 0)
               this.decomposeLeft(from, result);
           let left = result.length;
           for (let node of nodes)
               result.push(node);
           if (from > 0)
               mergeGaps(result, left - 1);
           if (to < this.length) {
               let right = result.length;
               this.decomposeRight(to, result);
               mergeGaps(result, right);
           }
           return HeightMap.of(result);
       }
       decomposeLeft(to, result) {
           let left = this.left.length;
           if (to <= left)
               return this.left.decomposeLeft(to, result);
           result.push(this.left);
           if (this.break) {
               left++;
               if (to >= left)
                   result.push(null);
           }
           if (to > left)
               this.right.decomposeLeft(to - left, result);
       }
       decomposeRight(from, result) {
           let left = this.left.length, right = left + this.break;
           if (from >= right)
               return this.right.decomposeRight(from - right, result);
           if (from < left)
               this.left.decomposeRight(from, result);
           if (this.break && from < right)
               result.push(null);
           result.push(this.right);
       }
       balanced(left, right) {
           if (left.size > 2 * right.size || right.size > 2 * left.size)
               return HeightMap.of(this.break ? [left, null, right] : [left, right]);
           this.left = left;
           this.right = right;
           this.height = left.height + right.height;
           this.outdated = left.outdated || right.outdated;
           this.size = left.size + right.size;
           this.length = left.length + this.break + right.length;
           return this;
       }
       updateHeight(oracle, offset = 0, force = false, measured) {
           let { left, right } = this, rightStart = offset + left.length + this.break, rebalance = null;
           if (measured && measured.from <= offset + left.length && measured.more)
               rebalance = left = left.updateHeight(oracle, offset, force, measured);
           else
               left.updateHeight(oracle, offset, force);
           if (measured && measured.from <= rightStart + right.length && measured.more)
               rebalance = right = right.updateHeight(oracle, rightStart, force, measured);
           else
               right.updateHeight(oracle, rightStart, force);
           if (rebalance)
               return this.balanced(left, right);
           this.height = this.left.height + this.right.height;
           this.outdated = false;
           return this;
       }
       toString() { return this.left + (this.break ? " " : "-") + this.right; }
   }
   function mergeGaps(nodes, around) {
       let before, after;
       if (nodes[around] == null &&
           (before = nodes[around - 1]) instanceof HeightMapGap &&
           (after = nodes[around + 1]) instanceof HeightMapGap)
           nodes.splice(around - 1, 3, new HeightMapGap(before.length + 1 + after.length));
   }
   const relevantWidgetHeight = 5;
   class NodeBuilder {
       constructor(pos, oracle) {
           this.pos = pos;
           this.oracle = oracle;
           this.nodes = [];
           this.lineStart = -1;
           this.lineEnd = -1;
           this.covering = null;
           this.writtenTo = pos;
       }
       get isCovered() {
           return this.covering && this.nodes[this.nodes.length - 1] == this.covering;
       }
       span(_from, to) {
           if (this.lineStart > -1) {
               let end = Math.min(to, this.lineEnd), last = this.nodes[this.nodes.length - 1];
               if (last instanceof HeightMapText)
                   last.length += end - this.pos;
               else if (end > this.pos || !this.isCovered)
                   this.nodes.push(new HeightMapText(end - this.pos, -1));
               this.writtenTo = end;
               if (to > end) {
                   this.nodes.push(null);
                   this.writtenTo++;
                   this.lineStart = -1;
               }
           }
           this.pos = to;
       }
       point(from, to, deco) {
           if (from < to || deco.heightRelevant) {
               let height = deco.widget ? deco.widget.estimatedHeight : 0;
               if (height < 0)
                   height = this.oracle.lineHeight;
               let len = to - from;
               if (deco.block) {
                   this.addBlock(new HeightMapBlock(len, height, deco.type));
               }
               else if (len || height >= relevantWidgetHeight) {
                   this.addLineDeco(height, len);
               }
           }
           else if (to > from) {
               this.span(from, to);
           }
           if (this.lineEnd > -1 && this.lineEnd < this.pos)
               this.lineEnd = this.oracle.doc.lineAt(this.pos).to;
       }
       enterLine() {
           if (this.lineStart > -1)
               return;
           let { from, to } = this.oracle.doc.lineAt(this.pos);
           this.lineStart = from;
           this.lineEnd = to;
           if (this.writtenTo < from) {
               if (this.writtenTo < from - 1 || this.nodes[this.nodes.length - 1] == null)
                   this.nodes.push(this.blankContent(this.writtenTo, from - 1));
               this.nodes.push(null);
           }
           if (this.pos > from)
               this.nodes.push(new HeightMapText(this.pos - from, -1));
           this.writtenTo = this.pos;
       }
       blankContent(from, to) {
           let gap = new HeightMapGap(to - from);
           if (this.oracle.doc.lineAt(from).to == to)
               gap.flags |= 4 /* Flag.SingleLine */;
           return gap;
       }
       ensureLine() {
           this.enterLine();
           let last = this.nodes.length ? this.nodes[this.nodes.length - 1] : null;
           if (last instanceof HeightMapText)
               return last;
           let line = new HeightMapText(0, -1);
           this.nodes.push(line);
           return line;
       }
       addBlock(block) {
           this.enterLine();
           if (block.type == BlockType.WidgetAfter && !this.isCovered)
               this.ensureLine();
           this.nodes.push(block);
           this.writtenTo = this.pos = this.pos + block.length;
           if (block.type != BlockType.WidgetBefore)
               this.covering = block;
       }
       addLineDeco(height, length) {
           let line = this.ensureLine();
           line.length += length;
           line.collapsed += length;
           line.widgetHeight = Math.max(line.widgetHeight, height);
           this.writtenTo = this.pos = this.pos + length;
       }
       finish(from) {
           let last = this.nodes.length == 0 ? null : this.nodes[this.nodes.length - 1];
           if (this.lineStart > -1 && !(last instanceof HeightMapText) && !this.isCovered)
               this.nodes.push(new HeightMapText(0, -1));
           else if (this.writtenTo < this.pos || last == null)
               this.nodes.push(this.blankContent(this.writtenTo, this.pos));
           let pos = from;
           for (let node of this.nodes) {
               if (node instanceof HeightMapText)
                   node.updateHeight(this.oracle, pos);
               pos += node ? node.length : 1;
           }
           return this.nodes;
       }
       // Always called with a region that on both sides either stretches
       // to a line break or the end of the document.
       // The returned array uses null to indicate line breaks, but never
       // starts or ends in a line break, or has multiple line breaks next
       // to each other.
       static build(oracle, decorations, from, to) {
           let builder = new NodeBuilder(from, oracle);
           RangeSet.spans(decorations, from, to, builder, 0);
           return builder.finish(from);
       }
   }
   function heightRelevantDecoChanges(a, b, diff) {
       let comp = new DecorationComparator;
       RangeSet.compare(a, b, diff, comp, 0);
       return comp.changes;
   }
   class DecorationComparator {
       constructor() {
           this.changes = [];
       }
       compareRange() { }
       comparePoint(from, to, a, b) {
           if (from < to || a && a.heightRelevant || b && b.heightRelevant)
               addRange(from, to, this.changes, 5);
       }
   }

   function visiblePixelRange(dom, paddingTop) {
       let rect = dom.getBoundingClientRect();
       let left = Math.max(0, rect.left), right = Math.min(innerWidth, rect.right);
       let top = Math.max(0, rect.top), bottom = Math.min(innerHeight, rect.bottom);
       let body = dom.ownerDocument.body;
       for (let parent = dom.parentNode; parent && parent != body;) {
           if (parent.nodeType == 1) {
               let elt = parent;
               let style = window.getComputedStyle(elt);
               if ((elt.scrollHeight > elt.clientHeight || elt.scrollWidth > elt.clientWidth) &&
                   style.overflow != "visible") {
                   let parentRect = elt.getBoundingClientRect();
                   left = Math.max(left, parentRect.left);
                   right = Math.min(right, parentRect.right);
                   top = Math.max(top, parentRect.top);
                   bottom = parent == dom.parentNode ? parentRect.bottom : Math.min(bottom, parentRect.bottom);
               }
               parent = style.position == "absolute" || style.position == "fixed" ? elt.offsetParent : elt.parentNode;
           }
           else if (parent.nodeType == 11) { // Shadow root
               parent = parent.host;
           }
           else {
               break;
           }
       }
       return { left: left - rect.left, right: Math.max(left, right) - rect.left,
           top: top - (rect.top + paddingTop), bottom: Math.max(top, bottom) - (rect.top + paddingTop) };
   }
   function fullPixelRange(dom, paddingTop) {
       let rect = dom.getBoundingClientRect();
       return { left: 0, right: rect.right - rect.left,
           top: paddingTop, bottom: rect.bottom - (rect.top + paddingTop) };
   }
   // Line gaps are placeholder widgets used to hide pieces of overlong
   // lines within the viewport, as a kludge to keep the editor
   // responsive when a ridiculously long line is loaded into it.
   class LineGap {
       constructor(from, to, size) {
           this.from = from;
           this.to = to;
           this.size = size;
       }
       static same(a, b) {
           if (a.length != b.length)
               return false;
           for (let i = 0; i < a.length; i++) {
               let gA = a[i], gB = b[i];
               if (gA.from != gB.from || gA.to != gB.to || gA.size != gB.size)
                   return false;
           }
           return true;
       }
       draw(wrapping) {
           return Decoration.replace({ widget: new LineGapWidget(this.size, wrapping) }).range(this.from, this.to);
       }
   }
   class LineGapWidget extends WidgetType {
       constructor(size, vertical) {
           super();
           this.size = size;
           this.vertical = vertical;
       }
       eq(other) { return other.size == this.size && other.vertical == this.vertical; }
       toDOM() {
           let elt = document.createElement("div");
           if (this.vertical) {
               elt.style.height = this.size + "px";
           }
           else {
               elt.style.width = this.size + "px";
               elt.style.height = "2px";
               elt.style.display = "inline-block";
           }
           return elt;
       }
       get estimatedHeight() { return this.vertical ? this.size : -1; }
   }
   class ViewState {
       constructor(state) {
           this.state = state;
           // These are contentDOM-local coordinates
           this.pixelViewport = { left: 0, right: window.innerWidth, top: 0, bottom: 0 };
           this.inView = true;
           this.paddingTop = 0;
           this.paddingBottom = 0;
           this.contentDOMWidth = 0;
           this.contentDOMHeight = 0;
           this.editorHeight = 0;
           this.editorWidth = 0;
           this.heightOracle = new HeightOracle;
           // See VP.MaxDOMHeight
           this.scaler = IdScaler;
           this.scrollTarget = null;
           // Briefly set to true when printing, to disable viewport limiting
           this.printing = false;
           // Flag set when editor content was redrawn, so that the next
           // measure stage knows it must read DOM layout
           this.mustMeasureContent = true;
           this.defaultTextDirection = Direction.RTL;
           this.visibleRanges = [];
           // Cursor 'assoc' is only significant when the cursor is on a line
           // wrap point, where it must stick to the character that it is
           // associated with. Since browsers don't provide a reasonable
           // interface to set or query this, when a selection is set that
           // might cause this to be significant, this flag is set. The next
           // measure phase will check whether the cursor is on a line-wrapping
           // boundary and, if so, reset it to make sure it is positioned in
           // the right place.
           this.mustEnforceCursorAssoc = false;
           this.stateDeco = state.facet(decorations).filter(d => typeof d != "function");
           this.heightMap = HeightMap.empty().applyChanges(this.stateDeco, Text.empty, this.heightOracle.setDoc(state.doc), [new ChangedRange(0, 0, 0, state.doc.length)]);
           this.viewport = this.getViewport(0, null);
           this.updateViewportLines();
           this.updateForViewport();
           this.lineGaps = this.ensureLineGaps([]);
           this.lineGapDeco = Decoration.set(this.lineGaps.map(gap => gap.draw(false)));
           this.computeVisibleRanges();
       }
       updateForViewport() {
           let viewports = [this.viewport], { main } = this.state.selection;
           for (let i = 0; i <= 1; i++) {
               let pos = i ? main.head : main.anchor;
               if (!viewports.some(({ from, to }) => pos >= from && pos <= to)) {
                   let { from, to } = this.lineBlockAt(pos);
                   viewports.push(new Viewport(from, to));
               }
           }
           this.viewports = viewports.sort((a, b) => a.from - b.from);
           this.scaler = this.heightMap.height <= 7000000 /* VP.MaxDOMHeight */ ? IdScaler :
               new BigScaler(this.heightOracle.doc, this.heightMap, this.viewports);
       }
       updateViewportLines() {
           this.viewportLines = [];
           this.heightMap.forEachLine(this.viewport.from, this.viewport.to, this.state.doc, 0, 0, block => {
               this.viewportLines.push(this.scaler.scale == 1 ? block : scaleBlock(block, this.scaler));
           });
       }
       update(update, scrollTarget = null) {
           this.state = update.state;
           let prevDeco = this.stateDeco;
           this.stateDeco = this.state.facet(decorations).filter(d => typeof d != "function");
           let contentChanges = update.changedRanges;
           let heightChanges = ChangedRange.extendWithRanges(contentChanges, heightRelevantDecoChanges(prevDeco, this.stateDeco, update ? update.changes : ChangeSet.empty(this.state.doc.length)));
           let prevHeight = this.heightMap.height;
           this.heightMap = this.heightMap.applyChanges(this.stateDeco, update.startState.doc, this.heightOracle.setDoc(this.state.doc), heightChanges);
           if (this.heightMap.height != prevHeight)
               update.flags |= 2 /* UpdateFlag.Height */;
           let viewport = heightChanges.length ? this.mapViewport(this.viewport, update.changes) : this.viewport;
           if (scrollTarget && (scrollTarget.range.head < viewport.from || scrollTarget.range.head > viewport.to) ||
               !this.viewportIsAppropriate(viewport))
               viewport = this.getViewport(0, scrollTarget);
           let updateLines = !update.changes.empty || (update.flags & 2 /* UpdateFlag.Height */) ||
               viewport.from != this.viewport.from || viewport.to != this.viewport.to;
           this.viewport = viewport;
           this.updateForViewport();
           if (updateLines)
               this.updateViewportLines();
           if (this.lineGaps.length || this.viewport.to - this.viewport.from > 4000 /* LG.DoubleMargin */)
               this.updateLineGaps(this.ensureLineGaps(this.mapLineGaps(this.lineGaps, update.changes)));
           update.flags |= this.computeVisibleRanges();
           if (scrollTarget)
               this.scrollTarget = scrollTarget;
           if (!this.mustEnforceCursorAssoc && update.selectionSet && update.view.lineWrapping &&
               update.state.selection.main.empty && update.state.selection.main.assoc)
               this.mustEnforceCursorAssoc = true;
       }
       measure(view) {
           let dom = view.contentDOM, style = window.getComputedStyle(dom);
           let oracle = this.heightOracle;
           let whiteSpace = style.whiteSpace;
           this.defaultTextDirection = style.direction == "rtl" ? Direction.RTL : Direction.LTR;
           let refresh = this.heightOracle.mustRefreshForWrapping(whiteSpace);
           let measureContent = refresh || this.mustMeasureContent || this.contentDOMHeight != dom.clientHeight;
           this.contentDOMHeight = dom.clientHeight;
           this.mustMeasureContent = false;
           let result = 0, bias = 0;
           // Vertical padding
           let paddingTop = parseInt(style.paddingTop) || 0, paddingBottom = parseInt(style.paddingBottom) || 0;
           if (this.paddingTop != paddingTop || this.paddingBottom != paddingBottom) {
               this.paddingTop = paddingTop;
               this.paddingBottom = paddingBottom;
               result |= 8 /* UpdateFlag.Geometry */ | 2 /* UpdateFlag.Height */;
           }
           if (this.editorWidth != view.scrollDOM.clientWidth) {
               if (oracle.lineWrapping)
                   measureContent = true;
               this.editorWidth = view.scrollDOM.clientWidth;
               result |= 8 /* UpdateFlag.Geometry */;
           }
           // Pixel viewport
           let pixelViewport = (this.printing ? fullPixelRange : visiblePixelRange)(dom, this.paddingTop);
           let dTop = pixelViewport.top - this.pixelViewport.top, dBottom = pixelViewport.bottom - this.pixelViewport.bottom;
           this.pixelViewport = pixelViewport;
           let inView = this.pixelViewport.bottom > this.pixelViewport.top && this.pixelViewport.right > this.pixelViewport.left;
           if (inView != this.inView) {
               this.inView = inView;
               if (inView)
                   measureContent = true;
           }
           if (!this.inView)
               return 0;
           let contentWidth = dom.clientWidth;
           if (this.contentDOMWidth != contentWidth || this.editorHeight != view.scrollDOM.clientHeight) {
               this.contentDOMWidth = contentWidth;
               this.editorHeight = view.scrollDOM.clientHeight;
               result |= 8 /* UpdateFlag.Geometry */;
           }
           if (measureContent) {
               let lineHeights = view.docView.measureVisibleLineHeights(this.viewport);
               if (oracle.mustRefreshForHeights(lineHeights))
                   refresh = true;
               if (refresh || oracle.lineWrapping && Math.abs(contentWidth - this.contentDOMWidth) > oracle.charWidth) {
                   let { lineHeight, charWidth } = view.docView.measureTextSize();
                   refresh = oracle.refresh(whiteSpace, lineHeight, charWidth, contentWidth / charWidth, lineHeights);
                   if (refresh) {
                       view.docView.minWidth = 0;
                       result |= 8 /* UpdateFlag.Geometry */;
                   }
               }
               if (dTop > 0 && dBottom > 0)
                   bias = Math.max(dTop, dBottom);
               else if (dTop < 0 && dBottom < 0)
                   bias = Math.min(dTop, dBottom);
               oracle.heightChanged = false;
               for (let vp of this.viewports) {
                   let heights = vp.from == this.viewport.from ? lineHeights : view.docView.measureVisibleLineHeights(vp);
                   this.heightMap = this.heightMap.updateHeight(oracle, 0, refresh, new MeasuredHeights(vp.from, heights));
               }
               if (oracle.heightChanged)
                   result |= 2 /* UpdateFlag.Height */;
           }
           let viewportChange = !this.viewportIsAppropriate(this.viewport, bias) ||
               this.scrollTarget && (this.scrollTarget.range.head < this.viewport.from || this.scrollTarget.range.head > this.viewport.to);
           if (viewportChange)
               this.viewport = this.getViewport(bias, this.scrollTarget);
           this.updateForViewport();
           if ((result & 2 /* UpdateFlag.Height */) || viewportChange)
               this.updateViewportLines();
           if (this.lineGaps.length || this.viewport.to - this.viewport.from > 4000 /* LG.DoubleMargin */)
               this.updateLineGaps(this.ensureLineGaps(refresh ? [] : this.lineGaps));
           result |= this.computeVisibleRanges();
           if (this.mustEnforceCursorAssoc) {
               this.mustEnforceCursorAssoc = false;
               // This is done in the read stage, because moving the selection
               // to a line end is going to trigger a layout anyway, so it
               // can't be a pure write. It should be rare that it does any
               // writing.
               view.docView.enforceCursorAssoc();
           }
           return result;
       }
       get visibleTop() { return this.scaler.fromDOM(this.pixelViewport.top); }
       get visibleBottom() { return this.scaler.fromDOM(this.pixelViewport.bottom); }
       getViewport(bias, scrollTarget) {
           // This will divide VP.Margin between the top and the
           // bottom, depending on the bias (the change in viewport position
           // since the last update). It'll hold a number between 0 and 1
           let marginTop = 0.5 - Math.max(-0.5, Math.min(0.5, bias / 1000 /* VP.Margin */ / 2));
           let map = this.heightMap, doc = this.state.doc, { visibleTop, visibleBottom } = this;
           let viewport = new Viewport(map.lineAt(visibleTop - marginTop * 1000 /* VP.Margin */, QueryType.ByHeight, doc, 0, 0).from, map.lineAt(visibleBottom + (1 - marginTop) * 1000 /* VP.Margin */, QueryType.ByHeight, doc, 0, 0).to);
           // If scrollTarget is given, make sure the viewport includes that position
           if (scrollTarget) {
               let { head } = scrollTarget.range;
               if (head < viewport.from || head > viewport.to) {
                   let viewHeight = Math.min(this.editorHeight, this.pixelViewport.bottom - this.pixelViewport.top);
                   let block = map.lineAt(head, QueryType.ByPos, doc, 0, 0), topPos;
                   if (scrollTarget.y == "center")
                       topPos = (block.top + block.bottom) / 2 - viewHeight / 2;
                   else if (scrollTarget.y == "start" || scrollTarget.y == "nearest" && head < viewport.from)
                       topPos = block.top;
                   else
                       topPos = block.bottom - viewHeight;
                   viewport = new Viewport(map.lineAt(topPos - 1000 /* VP.Margin */ / 2, QueryType.ByHeight, doc, 0, 0).from, map.lineAt(topPos + viewHeight + 1000 /* VP.Margin */ / 2, QueryType.ByHeight, doc, 0, 0).to);
               }
           }
           return viewport;
       }
       mapViewport(viewport, changes) {
           let from = changes.mapPos(viewport.from, -1), to = changes.mapPos(viewport.to, 1);
           return new Viewport(this.heightMap.lineAt(from, QueryType.ByPos, this.state.doc, 0, 0).from, this.heightMap.lineAt(to, QueryType.ByPos, this.state.doc, 0, 0).to);
       }
       // Checks if a given viewport covers the visible part of the
       // document and not too much beyond that.
       viewportIsAppropriate({ from, to }, bias = 0) {
           if (!this.inView)
               return true;
           let { top } = this.heightMap.lineAt(from, QueryType.ByPos, this.state.doc, 0, 0);
           let { bottom } = this.heightMap.lineAt(to, QueryType.ByPos, this.state.doc, 0, 0);
           let { visibleTop, visibleBottom } = this;
           return (from == 0 || top <= visibleTop - Math.max(10 /* VP.MinCoverMargin */, Math.min(-bias, 250 /* VP.MaxCoverMargin */))) &&
               (to == this.state.doc.length ||
                   bottom >= visibleBottom + Math.max(10 /* VP.MinCoverMargin */, Math.min(bias, 250 /* VP.MaxCoverMargin */))) &&
               (top > visibleTop - 2 * 1000 /* VP.Margin */ && bottom < visibleBottom + 2 * 1000 /* VP.Margin */);
       }
       mapLineGaps(gaps, changes) {
           if (!gaps.length || changes.empty)
               return gaps;
           let mapped = [];
           for (let gap of gaps)
               if (!changes.touchesRange(gap.from, gap.to))
                   mapped.push(new LineGap(changes.mapPos(gap.from), changes.mapPos(gap.to), gap.size));
           return mapped;
       }
       // Computes positions in the viewport where the start or end of a
       // line should be hidden, trying to reuse existing line gaps when
       // appropriate to avoid unneccesary redraws.
       // Uses crude character-counting for the positioning and sizing,
       // since actual DOM coordinates aren't always available and
       // predictable. Relies on generous margins (see LG.Margin) to hide
       // the artifacts this might produce from the user.
       ensureLineGaps(current) {
           let gaps = [];
           // This won't work at all in predominantly right-to-left text.
           if (this.defaultTextDirection != Direction.LTR)
               return gaps;
           for (let line of this.viewportLines) {
               if (line.length < 4000 /* LG.DoubleMargin */)
                   continue;
               let structure = lineStructure(line.from, line.to, this.stateDeco);
               if (structure.total < 4000 /* LG.DoubleMargin */)
                   continue;
               let viewFrom, viewTo;
               if (this.heightOracle.lineWrapping) {
                   let marginHeight = (2000 /* LG.Margin */ / this.heightOracle.lineLength) * this.heightOracle.lineHeight;
                   viewFrom = findPosition(structure, (this.visibleTop - line.top - marginHeight) / line.height);
                   viewTo = findPosition(structure, (this.visibleBottom - line.top + marginHeight) / line.height);
               }
               else {
                   let totalWidth = structure.total * this.heightOracle.charWidth;
                   let marginWidth = 2000 /* LG.Margin */ * this.heightOracle.charWidth;
                   viewFrom = findPosition(structure, (this.pixelViewport.left - marginWidth) / totalWidth);
                   viewTo = findPosition(structure, (this.pixelViewport.right + marginWidth) / totalWidth);
               }
               let outside = [];
               if (viewFrom > line.from)
                   outside.push({ from: line.from, to: viewFrom });
               if (viewTo < line.to)
                   outside.push({ from: viewTo, to: line.to });
               let sel = this.state.selection.main;
               // Make sure the gaps don't cover a selection end
               if (sel.from >= line.from && sel.from <= line.to)
                   cutRange(outside, sel.from - 10 /* LG.SelectionMargin */, sel.from + 10 /* LG.SelectionMargin */);
               if (!sel.empty && sel.to >= line.from && sel.to <= line.to)
                   cutRange(outside, sel.to - 10 /* LG.SelectionMargin */, sel.to + 10 /* LG.SelectionMargin */);
               for (let { from, to } of outside)
                   if (to - from > 1000 /* LG.HalfMargin */) {
                       gaps.push(find(current, gap => gap.from >= line.from && gap.to <= line.to &&
                           Math.abs(gap.from - from) < 1000 /* LG.HalfMargin */ && Math.abs(gap.to - to) < 1000 /* LG.HalfMargin */) ||
                           new LineGap(from, to, this.gapSize(line, from, to, structure)));
                   }
           }
           return gaps;
       }
       gapSize(line, from, to, structure) {
           let fraction = findFraction(structure, to) - findFraction(structure, from);
           if (this.heightOracle.lineWrapping) {
               return line.height * fraction;
           }
           else {
               return structure.total * this.heightOracle.charWidth * fraction;
           }
       }
       updateLineGaps(gaps) {
           if (!LineGap.same(gaps, this.lineGaps)) {
               this.lineGaps = gaps;
               this.lineGapDeco = Decoration.set(gaps.map(gap => gap.draw(this.heightOracle.lineWrapping)));
           }
       }
       computeVisibleRanges() {
           let deco = this.stateDeco;
           if (this.lineGaps.length)
               deco = deco.concat(this.lineGapDeco);
           let ranges = [];
           RangeSet.spans(deco, this.viewport.from, this.viewport.to, {
               span(from, to) { ranges.push({ from, to }); },
               point() { }
           }, 20);
           let changed = ranges.length != this.visibleRanges.length ||
               this.visibleRanges.some((r, i) => r.from != ranges[i].from || r.to != ranges[i].to);
           this.visibleRanges = ranges;
           return changed ? 4 /* UpdateFlag.Viewport */ : 0;
       }
       lineBlockAt(pos) {
           return (pos >= this.viewport.from && pos <= this.viewport.to && this.viewportLines.find(b => b.from <= pos && b.to >= pos)) ||
               scaleBlock(this.heightMap.lineAt(pos, QueryType.ByPos, this.state.doc, 0, 0), this.scaler);
       }
       lineBlockAtHeight(height) {
           return scaleBlock(this.heightMap.lineAt(this.scaler.fromDOM(height), QueryType.ByHeight, this.state.doc, 0, 0), this.scaler);
       }
       elementAtHeight(height) {
           return scaleBlock(this.heightMap.blockAt(this.scaler.fromDOM(height), this.state.doc, 0, 0), this.scaler);
       }
       get docHeight() {
           return this.scaler.toDOM(this.heightMap.height);
       }
       get contentHeight() {
           return this.docHeight + this.paddingTop + this.paddingBottom;
       }
   }
   class Viewport {
       constructor(from, to) {
           this.from = from;
           this.to = to;
       }
   }
   function lineStructure(from, to, stateDeco) {
       let ranges = [], pos = from, total = 0;
       RangeSet.spans(stateDeco, from, to, {
           span() { },
           point(from, to) {
               if (from > pos) {
                   ranges.push({ from: pos, to: from });
                   total += from - pos;
               }
               pos = to;
           }
       }, 20); // We're only interested in collapsed ranges of a significant size
       if (pos < to) {
           ranges.push({ from: pos, to });
           total += to - pos;
       }
       return { total, ranges };
   }
   function findPosition({ total, ranges }, ratio) {
       if (ratio <= 0)
           return ranges[0].from;
       if (ratio >= 1)
           return ranges[ranges.length - 1].to;
       let dist = Math.floor(total * ratio);
       for (let i = 0;; i++) {
           let { from, to } = ranges[i], size = to - from;
           if (dist <= size)
               return from + dist;
           dist -= size;
       }
   }
   function findFraction(structure, pos) {
       let counted = 0;
       for (let { from, to } of structure.ranges) {
           if (pos <= to) {
               counted += pos - from;
               break;
           }
           counted += to - from;
       }
       return counted / structure.total;
   }
   function cutRange(ranges, from, to) {
       for (let i = 0; i < ranges.length; i++) {
           let r = ranges[i];
           if (r.from < to && r.to > from) {
               let pieces = [];
               if (r.from < from)
                   pieces.push({ from: r.from, to: from });
               if (r.to > to)
                   pieces.push({ from: to, to: r.to });
               ranges.splice(i, 1, ...pieces);
               i += pieces.length - 1;
           }
       }
   }
   function find(array, f) {
       for (let val of array)
           if (f(val))
               return val;
       return undefined;
   }
   // Don't scale when the document height is within the range of what
   // the DOM can handle.
   const IdScaler = {
       toDOM(n) { return n; },
       fromDOM(n) { return n; },
       scale: 1
   };
   // When the height is too big (> VP.MaxDOMHeight), scale down the
   // regions outside the viewports so that the total height is
   // VP.MaxDOMHeight.
   class BigScaler {
       constructor(doc, heightMap, viewports) {
           let vpHeight = 0, base = 0, domBase = 0;
           this.viewports = viewports.map(({ from, to }) => {
               let top = heightMap.lineAt(from, QueryType.ByPos, doc, 0, 0).top;
               let bottom = heightMap.lineAt(to, QueryType.ByPos, doc, 0, 0).bottom;
               vpHeight += bottom - top;
               return { from, to, top, bottom, domTop: 0, domBottom: 0 };
           });
           this.scale = (7000000 /* VP.MaxDOMHeight */ - vpHeight) / (heightMap.height - vpHeight);
           for (let obj of this.viewports) {
               obj.domTop = domBase + (obj.top - base) * this.scale;
               domBase = obj.domBottom = obj.domTop + (obj.bottom - obj.top);
               base = obj.bottom;
           }
       }
       toDOM(n) {
           for (let i = 0, base = 0, domBase = 0;; i++) {
               let vp = i < this.viewports.length ? this.viewports[i] : null;
               if (!vp || n < vp.top)
                   return domBase + (n - base) * this.scale;
               if (n <= vp.bottom)
                   return vp.domTop + (n - vp.top);
               base = vp.bottom;
               domBase = vp.domBottom;
           }
       }
       fromDOM(n) {
           for (let i = 0, base = 0, domBase = 0;; i++) {
               let vp = i < this.viewports.length ? this.viewports[i] : null;
               if (!vp || n < vp.domTop)
                   return base + (n - domBase) / this.scale;
               if (n <= vp.domBottom)
                   return vp.top + (n - vp.domTop);
               base = vp.bottom;
               domBase = vp.domBottom;
           }
       }
   }
   function scaleBlock(block, scaler) {
       if (scaler.scale == 1)
           return block;
       let bTop = scaler.toDOM(block.top), bBottom = scaler.toDOM(block.bottom);
       return new BlockInfo(block.from, block.length, bTop, bBottom - bTop, Array.isArray(block.type) ? block.type.map(b => scaleBlock(b, scaler)) : block.type);
   }

   const theme = /*@__PURE__*/Facet.define({ combine: strs => strs.join(" ") });
   const darkTheme = /*@__PURE__*/Facet.define({ combine: values => values.indexOf(true) > -1 });
   const baseThemeID = /*@__PURE__*/StyleModule.newName(), baseLightID = /*@__PURE__*/StyleModule.newName(), baseDarkID = /*@__PURE__*/StyleModule.newName();
   const lightDarkIDs = { "&light": "." + baseLightID, "&dark": "." + baseDarkID };
   function buildTheme(main, spec, scopes) {
       return new StyleModule(spec, {
           finish(sel) {
               return /&/.test(sel) ? sel.replace(/&\w*/, m => {
                   if (m == "&")
                       return main;
                   if (!scopes || !scopes[m])
                       throw new RangeError(`Unsupported selector: ${m}`);
                   return scopes[m];
               }) : main + " " + sel;
           }
       });
   }
   const baseTheme$1 = /*@__PURE__*/buildTheme("." + baseThemeID, {
       "&.cm-editor": {
           position: "relative !important",
           boxSizing: "border-box",
           "&.cm-focused": {
               // Provide a simple default outline to make sure a focused
               // editor is visually distinct. Can't leave the default behavior
               // because that will apply to the content element, which is
               // inside the scrollable container and doesn't include the
               // gutters. We also can't use an 'auto' outline, since those
               // are, for some reason, drawn behind the element content, which
               // will cause things like the active line background to cover
               // the outline (#297).
               outline: "1px dotted #212121"
           },
           display: "flex !important",
           flexDirection: "column"
       },
       ".cm-scroller": {
           display: "flex !important",
           alignItems: "flex-start !important",
           fontFamily: "monospace",
           lineHeight: 1.4,
           height: "100%",
           overflowX: "auto",
           position: "relative",
           zIndex: 0
       },
       ".cm-content": {
           margin: 0,
           flexGrow: 2,
           flexShrink: 0,
           minHeight: "100%",
           display: "block",
           whiteSpace: "pre",
           wordWrap: "normal",
           boxSizing: "border-box",
           padding: "4px 0",
           outline: "none",
           "&[contenteditable=true]": {
               WebkitUserModify: "read-write-plaintext-only",
           }
       },
       ".cm-lineWrapping": {
           whiteSpace_fallback: "pre-wrap",
           whiteSpace: "break-spaces",
           wordBreak: "break-word",
           overflowWrap: "anywhere",
           flexShrink: 1
       },
       "&light .cm-content": { caretColor: "black" },
       "&dark .cm-content": { caretColor: "white" },
       ".cm-line": {
           display: "block",
           padding: "0 2px 0 4px"
       },
       ".cm-selectionLayer": {
           zIndex: -1,
           contain: "size style"
       },
       ".cm-selectionBackground": {
           position: "absolute",
       },
       "&light .cm-selectionBackground": {
           background: "#d9d9d9"
       },
       "&dark .cm-selectionBackground": {
           background: "#222"
       },
       "&light.cm-focused .cm-selectionBackground": {
           background: "#d7d4f0"
       },
       "&dark.cm-focused .cm-selectionBackground": {
           background: "#233"
       },
       ".cm-cursorLayer": {
           zIndex: 100,
           contain: "size style",
           pointerEvents: "none"
       },
       "&.cm-focused .cm-cursorLayer": {
           animation: "steps(1) cm-blink 1.2s infinite"
       },
       // Two animations defined so that we can switch between them to
       // restart the animation without forcing another style
       // recomputation.
       "@keyframes cm-blink": { "0%": {}, "50%": { opacity: 0 }, "100%": {} },
       "@keyframes cm-blink2": { "0%": {}, "50%": { opacity: 0 }, "100%": {} },
       ".cm-cursor, .cm-dropCursor": {
           position: "absolute",
           borderLeft: "1.2px solid black",
           marginLeft: "-0.6px",
           pointerEvents: "none",
       },
       ".cm-cursor": {
           display: "none"
       },
       "&dark .cm-cursor": {
           borderLeftColor: "#444"
       },
       "&.cm-focused .cm-cursor": {
           display: "block"
       },
       "&light .cm-activeLine": { backgroundColor: "#f3f9ff" },
       "&dark .cm-activeLine": { backgroundColor: "#223039" },
       "&light .cm-specialChar": { color: "red" },
       "&dark .cm-specialChar": { color: "#f78" },
       ".cm-gutters": {
           flexShrink: 0,
           display: "flex",
           height: "100%",
           boxSizing: "border-box",
           left: 0,
           zIndex: 200
       },
       "&light .cm-gutters": {
           backgroundColor: "#f5f5f5",
           color: "#6c6c6c",
           borderRight: "1px solid #ddd"
       },
       "&dark .cm-gutters": {
           backgroundColor: "#333338",
           color: "#ccc"
       },
       ".cm-gutter": {
           display: "flex !important",
           flexDirection: "column",
           flexShrink: 0,
           boxSizing: "border-box",
           minHeight: "100%",
           overflow: "hidden"
       },
       ".cm-gutterElement": {
           boxSizing: "border-box"
       },
       ".cm-lineNumbers .cm-gutterElement": {
           padding: "0 3px 0 5px",
           minWidth: "20px",
           textAlign: "right",
           whiteSpace: "nowrap"
       },
       "&light .cm-activeLineGutter": {
           backgroundColor: "#e2f2ff"
       },
       "&dark .cm-activeLineGutter": {
           backgroundColor: "#222227"
       },
       ".cm-panels": {
           boxSizing: "border-box",
           position: "sticky",
           left: 0,
           right: 0
       },
       "&light .cm-panels": {
           backgroundColor: "#f5f5f5",
           color: "black"
       },
       "&light .cm-panels-top": {
           borderBottom: "1px solid #ddd"
       },
       "&light .cm-panels-bottom": {
           borderTop: "1px solid #ddd"
       },
       "&dark .cm-panels": {
           backgroundColor: "#333338",
           color: "white"
       },
       ".cm-tab": {
           display: "inline-block",
           overflow: "hidden",
           verticalAlign: "bottom"
       },
       ".cm-widgetBuffer": {
           verticalAlign: "text-top",
           height: "1em",
           width: 0,
           display: "inline"
       },
       ".cm-placeholder": {
           color: "#888",
           display: "inline-block",
           verticalAlign: "top",
       },
       ".cm-button": {
           verticalAlign: "middle",
           color: "inherit",
           fontSize: "70%",
           padding: ".2em 1em",
           borderRadius: "1px"
       },
       "&light .cm-button": {
           backgroundImage: "linear-gradient(#eff1f5, #d9d9df)",
           border: "1px solid #888",
           "&:active": {
               backgroundImage: "linear-gradient(#b4b4b4, #d0d3d6)"
           }
       },
       "&dark .cm-button": {
           backgroundImage: "linear-gradient(#393939, #111)",
           border: "1px solid #888",
           "&:active": {
               backgroundImage: "linear-gradient(#111, #333)"
           }
       },
       ".cm-textfield": {
           verticalAlign: "middle",
           color: "inherit",
           fontSize: "70%",
           border: "1px solid silver",
           padding: ".2em .5em"
       },
       "&light .cm-textfield": {
           backgroundColor: "white"
       },
       "&dark .cm-textfield": {
           border: "1px solid #555",
           backgroundColor: "inherit"
       }
   }, lightDarkIDs);

   const observeOptions = {
       childList: true,
       characterData: true,
       subtree: true,
       attributes: true,
       characterDataOldValue: true
   };
   // IE11 has very broken mutation observers, so we also listen to
   // DOMCharacterDataModified there
   const useCharData = browser.ie && browser.ie_version <= 11;
   class DOMObserver {
       constructor(view, onChange, onScrollChanged) {
           this.view = view;
           this.onChange = onChange;
           this.onScrollChanged = onScrollChanged;
           this.active = false;
           // The known selection. Kept in our own object, as opposed to just
           // directly accessing the selection because:
           //  - Safari doesn't report the right selection in shadow DOM
           //  - Reading from the selection forces a DOM layout
           //  - This way, we can ignore selectionchange events if we have
           //    already seen the 'new' selection
           this.selectionRange = new DOMSelectionState;
           // Set when a selection change is detected, cleared on flush
           this.selectionChanged = false;
           this.delayedFlush = -1;
           this.resizeTimeout = -1;
           this.queue = [];
           this.delayedAndroidKey = null;
           this.lastChange = 0;
           this.scrollTargets = [];
           this.intersection = null;
           this.resize = null;
           this.intersecting = false;
           this.gapIntersection = null;
           this.gaps = [];
           // Timeout for scheduling check of the parents that need scroll handlers
           this.parentCheck = -1;
           this.dom = view.contentDOM;
           this.observer = new MutationObserver(mutations => {
               for (let mut of mutations)
                   this.queue.push(mut);
               // IE11 will sometimes (on typing over a selection or
               // backspacing out a single character text node) call the
               // observer callback before actually updating the DOM.
               //
               // Unrelatedly, iOS Safari will, when ending a composition,
               // sometimes first clear it, deliver the mutations, and then
               // reinsert the finished text. CodeMirror's handling of the
               // deletion will prevent the reinsertion from happening,
               // breaking composition.
               if ((browser.ie && browser.ie_version <= 11 || browser.ios && view.composing) &&
                   mutations.some(m => m.type == "childList" && m.removedNodes.length ||
                       m.type == "characterData" && m.oldValue.length > m.target.nodeValue.length))
                   this.flushSoon();
               else
                   this.flush();
           });
           if (useCharData)
               this.onCharData = (event) => {
                   this.queue.push({ target: event.target,
                       type: "characterData",
                       oldValue: event.prevValue });
                   this.flushSoon();
               };
           this.onSelectionChange = this.onSelectionChange.bind(this);
           this.onResize = this.onResize.bind(this);
           this.onPrint = this.onPrint.bind(this);
           this.onScroll = this.onScroll.bind(this);
           if (typeof ResizeObserver == "function") {
               this.resize = new ResizeObserver(() => {
                   if (this.view.docView.lastUpdate < Date.now() - 75)
                       this.onResize();
               });
               this.resize.observe(view.scrollDOM);
           }
           this.addWindowListeners(this.win = view.win);
           this.start();
           if (typeof IntersectionObserver == "function") {
               this.intersection = new IntersectionObserver(entries => {
                   if (this.parentCheck < 0)
                       this.parentCheck = setTimeout(this.listenForScroll.bind(this), 1000);
                   if (entries.length > 0 && (entries[entries.length - 1].intersectionRatio > 0) != this.intersecting) {
                       this.intersecting = !this.intersecting;
                       if (this.intersecting != this.view.inView)
                           this.onScrollChanged(document.createEvent("Event"));
                   }
               }, {});
               this.intersection.observe(this.dom);
               this.gapIntersection = new IntersectionObserver(entries => {
                   if (entries.length > 0 && entries[entries.length - 1].intersectionRatio > 0)
                       this.onScrollChanged(document.createEvent("Event"));
               }, {});
           }
           this.listenForScroll();
           this.readSelectionRange();
       }
       onScroll(e) {
           if (this.intersecting)
               this.flush(false);
           this.onScrollChanged(e);
       }
       onResize() {
           if (this.resizeTimeout < 0)
               this.resizeTimeout = setTimeout(() => {
                   this.resizeTimeout = -1;
                   this.view.requestMeasure();
               }, 50);
       }
       onPrint() {
           this.view.viewState.printing = true;
           this.view.measure();
           setTimeout(() => {
               this.view.viewState.printing = false;
               this.view.requestMeasure();
           }, 500);
       }
       updateGaps(gaps) {
           if (this.gapIntersection && (gaps.length != this.gaps.length || this.gaps.some((g, i) => g != gaps[i]))) {
               this.gapIntersection.disconnect();
               for (let gap of gaps)
                   this.gapIntersection.observe(gap);
               this.gaps = gaps;
           }
       }
       onSelectionChange(event) {
           let wasChanged = this.selectionChanged;
           if (!this.readSelectionRange() || this.delayedAndroidKey)
               return;
           let { view } = this, sel = this.selectionRange;
           if (view.state.facet(editable) ? view.root.activeElement != this.dom : !hasSelection(view.dom, sel))
               return;
           let context = sel.anchorNode && view.docView.nearest(sel.anchorNode);
           if (context && context.ignoreEvent(event)) {
               if (!wasChanged)
                   this.selectionChanged = false;
               return;
           }
           // Deletions on IE11 fire their events in the wrong order, giving
           // us a selection change event before the DOM changes are
           // reported.
           // Chrome Android has a similar issue when backspacing out a
           // selection (#645).
           if ((browser.ie && browser.ie_version <= 11 || browser.android && browser.chrome) && !view.state.selection.main.empty &&
               // (Selection.isCollapsed isn't reliable on IE)
               sel.focusNode && isEquivalentPosition(sel.focusNode, sel.focusOffset, sel.anchorNode, sel.anchorOffset))
               this.flushSoon();
           else
               this.flush(false);
       }
       readSelectionRange() {
           let { view } = this;
           // The Selection object is broken in shadow roots in Safari. See
           // https://github.com/codemirror/dev/issues/414
           let range = browser.safari && view.root.nodeType == 11 &&
               deepActiveElement(this.dom.ownerDocument) == this.dom &&
               safariSelectionRangeHack(this.view) || getSelection(view.root);
           if (!range || this.selectionRange.eq(range))
               return false;
           let local = hasSelection(this.dom, range);
           // Detect the situation where the browser has, on focus, moved the
           // selection to the start of the content element. Reset it to the
           // position from the editor state.
           if (local && !this.selectionChanged &&
               view.inputState.lastFocusTime > Date.now() - 200 &&
               view.inputState.lastTouchTime < Date.now() - 300 &&
               atElementStart(this.dom, range)) {
               this.view.inputState.lastFocusTime = 0;
               view.docView.updateSelection();
               return false;
           }
           this.selectionRange.setRange(range);
           if (local)
               this.selectionChanged = true;
           return true;
       }
       setSelectionRange(anchor, head) {
           this.selectionRange.set(anchor.node, anchor.offset, head.node, head.offset);
           this.selectionChanged = false;
       }
       clearSelectionRange() {
           this.selectionRange.set(null, 0, null, 0);
       }
       listenForScroll() {
           this.parentCheck = -1;
           let i = 0, changed = null;
           for (let dom = this.dom; dom;) {
               if (dom.nodeType == 1) {
                   if (!changed && i < this.scrollTargets.length && this.scrollTargets[i] == dom)
                       i++;
                   else if (!changed)
                       changed = this.scrollTargets.slice(0, i);
                   if (changed)
                       changed.push(dom);
                   dom = dom.assignedSlot || dom.parentNode;
               }
               else if (dom.nodeType == 11) { // Shadow root
                   dom = dom.host;
               }
               else {
                   break;
               }
           }
           if (i < this.scrollTargets.length && !changed)
               changed = this.scrollTargets.slice(0, i);
           if (changed) {
               for (let dom of this.scrollTargets)
                   dom.removeEventListener("scroll", this.onScroll);
               for (let dom of this.scrollTargets = changed)
                   dom.addEventListener("scroll", this.onScroll);
           }
       }
       ignore(f) {
           if (!this.active)
               return f();
           try {
               this.stop();
               return f();
           }
           finally {
               this.start();
               this.clear();
           }
       }
       start() {
           if (this.active)
               return;
           this.observer.observe(this.dom, observeOptions);
           if (useCharData)
               this.dom.addEventListener("DOMCharacterDataModified", this.onCharData);
           this.active = true;
       }
       stop() {
           if (!this.active)
               return;
           this.active = false;
           this.observer.disconnect();
           if (useCharData)
               this.dom.removeEventListener("DOMCharacterDataModified", this.onCharData);
       }
       // Throw away any pending changes
       clear() {
           this.processRecords();
           this.queue.length = 0;
           this.selectionChanged = false;
       }
       // Chrome Android, especially in combination with GBoard, not only
       // doesn't reliably fire regular key events, but also often
       // surrounds the effect of enter or backspace with a bunch of
       // composition events that, when interrupted, cause text duplication
       // or other kinds of corruption. This hack makes the editor back off
       // from handling DOM changes for a moment when such a key is
       // detected (via beforeinput or keydown), and then tries to flush
       // them or, if that has no effect, dispatches the given key.
       delayAndroidKey(key, keyCode) {
           var _a;
           if (!this.delayedAndroidKey)
               this.view.win.requestAnimationFrame(() => {
                   let key = this.delayedAndroidKey;
                   this.delayedAndroidKey = null;
                   this.delayedFlush = -1;
                   if (!this.flush() && key.force)
                       dispatchKey(this.dom, key.key, key.keyCode);
               });
           // Since backspace beforeinput is sometimes signalled spuriously,
           // Enter always takes precedence.
           if (!this.delayedAndroidKey || key == "Enter")
               this.delayedAndroidKey = {
                   key, keyCode,
                   // Only run the key handler when no changes are detected if
                   // this isn't coming right after another change, in which case
                   // it is probably part of a weird chain of updates, and should
                   // be ignored if it returns the DOM to its previous state.
                   force: this.lastChange < Date.now() - 50 || !!((_a = this.delayedAndroidKey) === null || _a === void 0 ? void 0 : _a.force)
               };
       }
       flushSoon() {
           if (this.delayedFlush < 0)
               this.delayedFlush = this.view.win.requestAnimationFrame(() => { this.delayedFlush = -1; this.flush(); });
       }
       forceFlush() {
           if (this.delayedFlush >= 0) {
               this.view.win.cancelAnimationFrame(this.delayedFlush);
               this.delayedFlush = -1;
           }
           this.flush();
       }
       processRecords() {
           let records = this.queue;
           for (let mut of this.observer.takeRecords())
               records.push(mut);
           if (records.length)
               this.queue = [];
           let from = -1, to = -1, typeOver = false;
           for (let record of records) {
               let range = this.readMutation(record);
               if (!range)
                   continue;
               if (range.typeOver)
                   typeOver = true;
               if (from == -1) {
                   ({ from, to } = range);
               }
               else {
                   from = Math.min(range.from, from);
                   to = Math.max(range.to, to);
               }
           }
           return { from, to, typeOver };
       }
       // Apply pending changes, if any
       flush(readSelection = true) {
           // Completely hold off flushing when pending keys are set—the code
           // managing those will make sure processRecords is called and the
           // view is resynchronized after
           if (this.delayedFlush >= 0 || this.delayedAndroidKey)
               return false;
           if (readSelection)
               this.readSelectionRange();
           let { from, to, typeOver } = this.processRecords();
           let newSel = this.selectionChanged && hasSelection(this.dom, this.selectionRange);
           if (from < 0 && !newSel)
               return false;
           if (from > -1)
               this.lastChange = Date.now();
           this.view.inputState.lastFocusTime = 0;
           this.selectionChanged = false;
           let startState = this.view.state;
           let handled = this.onChange(from, to, typeOver);
           // The view wasn't updated
           if (this.view.state == startState)
               this.view.update([]);
           return handled;
       }
       readMutation(rec) {
           let cView = this.view.docView.nearest(rec.target);
           if (!cView || cView.ignoreMutation(rec))
               return null;
           cView.markDirty(rec.type == "attributes");
           if (rec.type == "attributes")
               cView.dirty |= 4 /* Dirty.Attrs */;
           if (rec.type == "childList") {
               let childBefore = findChild(cView, rec.previousSibling || rec.target.previousSibling, -1);
               let childAfter = findChild(cView, rec.nextSibling || rec.target.nextSibling, 1);
               return { from: childBefore ? cView.posAfter(childBefore) : cView.posAtStart,
                   to: childAfter ? cView.posBefore(childAfter) : cView.posAtEnd, typeOver: false };
           }
           else if (rec.type == "characterData") {
               return { from: cView.posAtStart, to: cView.posAtEnd, typeOver: rec.target.nodeValue == rec.oldValue };
           }
           else {
               return null;
           }
       }
       setWindow(win) {
           if (win != this.win) {
               this.removeWindowListeners(this.win);
               this.win = win;
               this.addWindowListeners(this.win);
           }
       }
       addWindowListeners(win) {
           win.addEventListener("resize", this.onResize);
           win.addEventListener("beforeprint", this.onPrint);
           win.addEventListener("scroll", this.onScroll);
           win.document.addEventListener("selectionchange", this.onSelectionChange);
       }
       removeWindowListeners(win) {
           win.removeEventListener("scroll", this.onScroll);
           win.removeEventListener("resize", this.onResize);
           win.removeEventListener("beforeprint", this.onPrint);
           win.document.removeEventListener("selectionchange", this.onSelectionChange);
       }
       destroy() {
           var _a, _b, _c;
           this.stop();
           (_a = this.intersection) === null || _a === void 0 ? void 0 : _a.disconnect();
           (_b = this.gapIntersection) === null || _b === void 0 ? void 0 : _b.disconnect();
           (_c = this.resize) === null || _c === void 0 ? void 0 : _c.disconnect();
           for (let dom of this.scrollTargets)
               dom.removeEventListener("scroll", this.onScroll);
           this.removeWindowListeners(this.win);
           clearTimeout(this.parentCheck);
           clearTimeout(this.resizeTimeout);
       }
   }
   function findChild(cView, dom, dir) {
       while (dom) {
           let curView = ContentView.get(dom);
           if (curView && curView.parent == cView)
               return curView;
           let parent = dom.parentNode;
           dom = parent != cView.dom ? parent : dir > 0 ? dom.nextSibling : dom.previousSibling;
       }
       return null;
   }
   // Used to work around a Safari Selection/shadow DOM bug (#414)
   function safariSelectionRangeHack(view) {
       let found = null;
       // Because Safari (at least in 2018-2021) doesn't provide regular
       // access to the selection inside a shadowroot, we have to perform a
       // ridiculous hack to get at it—using `execCommand` to trigger a
       // `beforeInput` event so that we can read the target range from the
       // event.
       function read(event) {
           event.preventDefault();
           event.stopImmediatePropagation();
           found = event.getTargetRanges()[0];
       }
       view.contentDOM.addEventListener("beforeinput", read, true);
       view.dom.ownerDocument.execCommand("indent");
       view.contentDOM.removeEventListener("beforeinput", read, true);
       if (!found)
           return null;
       let anchorNode = found.startContainer, anchorOffset = found.startOffset;
       let focusNode = found.endContainer, focusOffset = found.endOffset;
       let curAnchor = view.docView.domAtPos(view.state.selection.main.anchor);
       // Since such a range doesn't distinguish between anchor and head,
       // use a heuristic that flips it around if its end matches the
       // current anchor.
       if (isEquivalentPosition(curAnchor.node, curAnchor.offset, focusNode, focusOffset))
           [anchorNode, anchorOffset, focusNode, focusOffset] = [focusNode, focusOffset, anchorNode, anchorOffset];
       return { anchorNode, anchorOffset, focusNode, focusOffset };
   }

   function applyDOMChange(view, start, end, typeOver) {
       let change, newSel;
       let sel = view.state.selection.main;
       if (start > -1) {
           let bounds = view.docView.domBoundsAround(start, end, 0);
           if (!bounds || view.state.readOnly)
               return false;
           let { from, to } = bounds;
           let selPoints = view.docView.impreciseHead || view.docView.impreciseAnchor ? [] : selectionPoints(view);
           let reader = new DOMReader(selPoints, view.state);
           reader.readRange(bounds.startDOM, bounds.endDOM);
           let preferredPos = sel.from, preferredSide = null;
           // Prefer anchoring to end when Backspace is pressed (or, on
           // Android, when something was deleted)
           if (view.inputState.lastKeyCode === 8 && view.inputState.lastKeyTime > Date.now() - 100 ||
               browser.android && reader.text.length < to - from) {
               preferredPos = sel.to;
               preferredSide = "end";
           }
           let diff = findDiff(view.state.doc.sliceString(from, to, LineBreakPlaceholder), reader.text, preferredPos - from, preferredSide);
           if (diff) {
               // Chrome inserts two newlines when pressing shift-enter at the
               // end of a line. This drops one of those.
               if (browser.chrome && view.inputState.lastKeyCode == 13 &&
                   diff.toB == diff.from + 2 && reader.text.slice(diff.from, diff.toB) == LineBreakPlaceholder + LineBreakPlaceholder)
                   diff.toB--;
               change = { from: from + diff.from, to: from + diff.toA,
                   insert: Text.of(reader.text.slice(diff.from, diff.toB).split(LineBreakPlaceholder)) };
           }
           newSel = selectionFromPoints(selPoints, from);
       }
       else if (view.hasFocus || !view.state.facet(editable)) {
           let domSel = view.observer.selectionRange;
           let { impreciseHead: iHead, impreciseAnchor: iAnchor } = view.docView;
           let head = iHead && iHead.node == domSel.focusNode && iHead.offset == domSel.focusOffset ||
               !contains(view.contentDOM, domSel.focusNode)
               ? view.state.selection.main.head
               : view.docView.posFromDOM(domSel.focusNode, domSel.focusOffset);
           let anchor = iAnchor && iAnchor.node == domSel.anchorNode && iAnchor.offset == domSel.anchorOffset ||
               !contains(view.contentDOM, domSel.anchorNode)
               ? view.state.selection.main.anchor
               : view.docView.posFromDOM(domSel.anchorNode, domSel.anchorOffset);
           if (head != sel.head || anchor != sel.anchor)
               newSel = EditorSelection.single(anchor, head);
       }
       if (!change && !newSel)
           return false;
       // Heuristic to notice typing over a selected character
       if (!change && typeOver && !sel.empty && newSel && newSel.main.empty)
           change = { from: sel.from, to: sel.to, insert: view.state.doc.slice(sel.from, sel.to) };
       // If the change is inside the selection and covers most of it,
       // assume it is a selection replace (with identical characters at
       // the start/end not included in the diff)
       else if (change && change.from >= sel.from && change.to <= sel.to &&
           (change.from != sel.from || change.to != sel.to) &&
           (sel.to - sel.from) - (change.to - change.from) <= 4)
           change = {
               from: sel.from, to: sel.to,
               insert: view.state.doc.slice(sel.from, change.from).append(change.insert).append(view.state.doc.slice(change.to, sel.to))
           };
       // Detect insert-period-on-double-space Mac behavior, and transform
       // it into a regular space insert.
       else if ((browser.mac || browser.android) && change && change.from == change.to && change.from == sel.head - 1 &&
           change.insert.toString() == ".")
           change = { from: sel.from, to: sel.to, insert: Text.of([" "]) };
       if (change) {
           let startState = view.state;
           if (browser.ios && view.inputState.flushIOSKey(view))
               return true;
           // Android browsers don't fire reasonable key events for enter,
           // backspace, or delete. So this detects changes that look like
           // they're caused by those keys, and reinterprets them as key
           // events. (Some of these keys are also handled by beforeinput
           // events and the pendingAndroidKey mechanism, but that's not
           // reliable in all situations.)
           if (browser.android &&
               ((change.from == sel.from && change.to == sel.to &&
                   change.insert.length == 1 && change.insert.lines == 2 &&
                   dispatchKey(view.contentDOM, "Enter", 13)) ||
                   (change.from == sel.from - 1 && change.to == sel.to && change.insert.length == 0 &&
                       dispatchKey(view.contentDOM, "Backspace", 8)) ||
                   (change.from == sel.from && change.to == sel.to + 1 && change.insert.length == 0 &&
                       dispatchKey(view.contentDOM, "Delete", 46))))
               return true;
           let text = change.insert.toString();
           if (view.state.facet(inputHandler).some(h => h(view, change.from, change.to, text)))
               return true;
           if (view.inputState.composing >= 0)
               view.inputState.composing++;
           let tr;
           if (change.from >= sel.from && change.to <= sel.to && change.to - change.from >= (sel.to - sel.from) / 3 &&
               (!newSel || newSel.main.empty && newSel.main.from == change.from + change.insert.length) &&
               view.inputState.composing < 0) {
               let before = sel.from < change.from ? startState.sliceDoc(sel.from, change.from) : "";
               let after = sel.to > change.to ? startState.sliceDoc(change.to, sel.to) : "";
               tr = startState.replaceSelection(view.state.toText(before + change.insert.sliceString(0, undefined, view.state.lineBreak) + after));
           }
           else {
               let changes = startState.changes(change);
               let mainSel = newSel && !startState.selection.main.eq(newSel.main) && newSel.main.to <= changes.newLength
                   ? newSel.main : undefined;
               // Try to apply a composition change to all cursors
               if (startState.selection.ranges.length > 1 && view.inputState.composing >= 0 &&
                   change.to <= sel.to && change.to >= sel.to - 10) {
                   let replaced = view.state.sliceDoc(change.from, change.to);
                   let compositionRange = compositionSurroundingNode(view) || view.state.doc.lineAt(sel.head);
                   let offset = sel.to - change.to, size = sel.to - sel.from;
                   tr = startState.changeByRange(range => {
                       if (range.from == sel.from && range.to == sel.to)
                           return { changes, range: mainSel || range.map(changes) };
                       let to = range.to - offset, from = to - replaced.length;
                       if (range.to - range.from != size || view.state.sliceDoc(from, to) != replaced ||
                           // Unfortunately, there's no way to make multiple
                           // changes in the same node work without aborting
                           // composition, so cursors in the composition range are
                           // ignored.
                           compositionRange && range.to >= compositionRange.from && range.from <= compositionRange.to)
                           return { range };
                       let rangeChanges = startState.changes({ from, to, insert: change.insert }), selOff = range.to - sel.to;
                       return {
                           changes: rangeChanges,
                           range: !mainSel ? range.map(rangeChanges) :
                               EditorSelection.range(Math.max(0, mainSel.anchor + selOff), Math.max(0, mainSel.head + selOff))
                       };
                   });
               }
               else {
                   tr = {
                       changes,
                       selection: mainSel && startState.selection.replaceRange(mainSel)
                   };
               }
           }
           let userEvent = "input.type";
           if (view.composing) {
               userEvent += ".compose";
               if (view.inputState.compositionFirstChange) {
                   userEvent += ".start";
                   view.inputState.compositionFirstChange = false;
               }
           }
           view.dispatch(tr, { scrollIntoView: true, userEvent });
           return true;
       }
       else if (newSel && !newSel.main.eq(sel)) {
           let scrollIntoView = false, userEvent = "select";
           if (view.inputState.lastSelectionTime > Date.now() - 50) {
               if (view.inputState.lastSelectionOrigin == "select")
                   scrollIntoView = true;
               userEvent = view.inputState.lastSelectionOrigin;
           }
           view.dispatch({ selection: newSel, scrollIntoView, userEvent });
           return true;
       }
       else {
           return false;
       }
   }
   function findDiff(a, b, preferredPos, preferredSide) {
       let minLen = Math.min(a.length, b.length);
       let from = 0;
       while (from < minLen && a.charCodeAt(from) == b.charCodeAt(from))
           from++;
       if (from == minLen && a.length == b.length)
           return null;
       let toA = a.length, toB = b.length;
       while (toA > 0 && toB > 0 && a.charCodeAt(toA - 1) == b.charCodeAt(toB - 1)) {
           toA--;
           toB--;
       }
       if (preferredSide == "end") {
           let adjust = Math.max(0, from - Math.min(toA, toB));
           preferredPos -= toA + adjust - from;
       }
       if (toA < from && a.length < b.length) {
           let move = preferredPos <= from && preferredPos >= toA ? from - preferredPos : 0;
           from -= move;
           toB = from + (toB - toA);
           toA = from;
       }
       else if (toB < from) {
           let move = preferredPos <= from && preferredPos >= toB ? from - preferredPos : 0;
           from -= move;
           toA = from + (toA - toB);
           toB = from;
       }
       return { from, toA, toB };
   }
   function selectionPoints(view) {
       let result = [];
       if (view.root.activeElement != view.contentDOM)
           return result;
       let { anchorNode, anchorOffset, focusNode, focusOffset } = view.observer.selectionRange;
       if (anchorNode) {
           result.push(new DOMPoint(anchorNode, anchorOffset));
           if (focusNode != anchorNode || focusOffset != anchorOffset)
               result.push(new DOMPoint(focusNode, focusOffset));
       }
       return result;
   }
   function selectionFromPoints(points, base) {
       if (points.length == 0)
           return null;
       let anchor = points[0].pos, head = points.length == 2 ? points[1].pos : anchor;
       return anchor > -1 && head > -1 ? EditorSelection.single(anchor + base, head + base) : null;
   }

   // The editor's update state machine looks something like this:
   //
   //     Idle → Updating ⇆ Idle (unchecked) → Measuring → Idle
   //                                         ↑      ↓
   //                                         Updating (measure)
   //
   // The difference between 'Idle' and 'Idle (unchecked)' lies in
   // whether a layout check has been scheduled. A regular update through
   // the `update` method updates the DOM in a write-only fashion, and
   // relies on a check (scheduled with `requestAnimationFrame`) to make
   // sure everything is where it should be and the viewport covers the
   // visible code. That check continues to measure and then optionally
   // update until it reaches a coherent state.
   /**
   An editor view represents the editor's user interface. It holds
   the editable DOM surface, and possibly other elements such as the
   line number gutter. It handles events and dispatches state
   transactions for editing actions.
   */
   class EditorView {
       /**
       Construct a new view. You'll want to either provide a `parent`
       option, or put `view.dom` into your document after creating a
       view, so that the user can see the editor.
       */
       constructor(config = {}) {
           this.plugins = [];
           this.pluginMap = new Map;
           this.editorAttrs = {};
           this.contentAttrs = {};
           this.bidiCache = [];
           this.destroyed = false;
           /**
           @internal
           */
           this.updateState = 2 /* UpdateState.Updating */;
           /**
           @internal
           */
           this.measureScheduled = -1;
           /**
           @internal
           */
           this.measureRequests = [];
           this.contentDOM = document.createElement("div");
           this.scrollDOM = document.createElement("div");
           this.scrollDOM.tabIndex = -1;
           this.scrollDOM.className = "cm-scroller";
           this.scrollDOM.appendChild(this.contentDOM);
           this.announceDOM = document.createElement("div");
           this.announceDOM.style.cssText = "position: absolute; top: -10000px";
           this.announceDOM.setAttribute("aria-live", "polite");
           this.dom = document.createElement("div");
           this.dom.appendChild(this.announceDOM);
           this.dom.appendChild(this.scrollDOM);
           this._dispatch = config.dispatch || ((tr) => this.update([tr]));
           this.dispatch = this.dispatch.bind(this);
           this._root = (config.root || getRoot(config.parent) || document);
           this.viewState = new ViewState(config.state || EditorState.create(config));
           this.plugins = this.state.facet(viewPlugin).map(spec => new PluginInstance(spec));
           for (let plugin of this.plugins)
               plugin.update(this);
           this.observer = new DOMObserver(this, (from, to, typeOver) => {
               return applyDOMChange(this, from, to, typeOver);
           }, event => {
               this.inputState.runScrollHandlers(this, event);
               if (this.observer.intersecting)
                   this.measure();
           });
           this.inputState = new InputState(this);
           this.inputState.ensureHandlers(this, this.plugins);
           this.docView = new DocView(this);
           this.mountStyles();
           this.updateAttrs();
           this.updateState = 0 /* UpdateState.Idle */;
           this.requestMeasure();
           if (config.parent)
               config.parent.appendChild(this.dom);
       }
       /**
       The current editor state.
       */
       get state() { return this.viewState.state; }
       /**
       To be able to display large documents without consuming too much
       memory or overloading the browser, CodeMirror only draws the
       code that is visible (plus a margin around it) to the DOM. This
       property tells you the extent of the current drawn viewport, in
       document positions.
       */
       get viewport() { return this.viewState.viewport; }
       /**
       When there are, for example, large collapsed ranges in the
       viewport, its size can be a lot bigger than the actual visible
       content. Thus, if you are doing something like styling the
       content in the viewport, it is preferable to only do so for
       these ranges, which are the subset of the viewport that is
       actually drawn.
       */
       get visibleRanges() { return this.viewState.visibleRanges; }
       /**
       Returns false when the editor is entirely scrolled out of view
       or otherwise hidden.
       */
       get inView() { return this.viewState.inView; }
       /**
       Indicates whether the user is currently composing text via
       [IME](https://en.wikipedia.org/wiki/Input_method), and at least
       one change has been made in the current composition.
       */
       get composing() { return this.inputState.composing > 0; }
       /**
       Indicates whether the user is currently in composing state. Note
       that on some platforms, like Android, this will be the case a
       lot, since just putting the cursor on a word starts a
       composition there.
       */
       get compositionStarted() { return this.inputState.composing >= 0; }
       /**
       The document or shadow root that the view lives in.
       */
       get root() { return this._root; }
       /**
       @internal
       */
       get win() { return this.dom.ownerDocument.defaultView; }
       dispatch(...input) {
           this._dispatch(input.length == 1 && input[0] instanceof Transaction ? input[0]
               : this.state.update(...input));
       }
       /**
       Update the view for the given array of transactions. This will
       update the visible document and selection to match the state
       produced by the transactions, and notify view plugins of the
       change. You should usually call
       [`dispatch`](https://codemirror.net/6/docs/ref/#view.EditorView.dispatch) instead, which uses this
       as a primitive.
       */
       update(transactions) {
           if (this.updateState != 0 /* UpdateState.Idle */)
               throw new Error("Calls to EditorView.update are not allowed while an update is in progress");
           let redrawn = false, attrsChanged = false, update;
           let state = this.state;
           for (let tr of transactions) {
               if (tr.startState != state)
                   throw new RangeError("Trying to update state with a transaction that doesn't start from the previous state.");
               state = tr.state;
           }
           if (this.destroyed) {
               this.viewState.state = state;
               return;
           }
           this.observer.clear();
           // When the phrases change, redraw the editor
           if (state.facet(EditorState.phrases) != this.state.facet(EditorState.phrases))
               return this.setState(state);
           update = ViewUpdate.create(this, state, transactions);
           let scrollTarget = this.viewState.scrollTarget;
           try {
               this.updateState = 2 /* UpdateState.Updating */;
               for (let tr of transactions) {
                   if (scrollTarget)
                       scrollTarget = scrollTarget.map(tr.changes);
                   if (tr.scrollIntoView) {
                       let { main } = tr.state.selection;
                       scrollTarget = new ScrollTarget(main.empty ? main : EditorSelection.cursor(main.head, main.head > main.anchor ? -1 : 1));
                   }
                   for (let e of tr.effects)
                       if (e.is(scrollIntoView))
                           scrollTarget = e.value;
               }
               this.viewState.update(update, scrollTarget);
               this.bidiCache = CachedOrder.update(this.bidiCache, update.changes);
               if (!update.empty) {
                   this.updatePlugins(update);
                   this.inputState.update(update);
               }
               redrawn = this.docView.update(update);
               if (this.state.facet(styleModule) != this.styleModules)
                   this.mountStyles();
               attrsChanged = this.updateAttrs();
               this.showAnnouncements(transactions);
               this.docView.updateSelection(redrawn, transactions.some(tr => tr.isUserEvent("select.pointer")));
           }
           finally {
               this.updateState = 0 /* UpdateState.Idle */;
           }
           if (update.startState.facet(theme) != update.state.facet(theme))
               this.viewState.mustMeasureContent = true;
           if (redrawn || attrsChanged || scrollTarget || this.viewState.mustEnforceCursorAssoc || this.viewState.mustMeasureContent)
               this.requestMeasure();
           if (!update.empty)
               for (let listener of this.state.facet(updateListener))
                   listener(update);
       }
       /**
       Reset the view to the given state. (This will cause the entire
       document to be redrawn and all view plugins to be reinitialized,
       so you should probably only use it when the new state isn't
       derived from the old state. Otherwise, use
       [`dispatch`](https://codemirror.net/6/docs/ref/#view.EditorView.dispatch) instead.)
       */
       setState(newState) {
           if (this.updateState != 0 /* UpdateState.Idle */)
               throw new Error("Calls to EditorView.setState are not allowed while an update is in progress");
           if (this.destroyed) {
               this.viewState.state = newState;
               return;
           }
           this.updateState = 2 /* UpdateState.Updating */;
           let hadFocus = this.hasFocus;
           try {
               for (let plugin of this.plugins)
                   plugin.destroy(this);
               this.viewState = new ViewState(newState);
               this.plugins = newState.facet(viewPlugin).map(spec => new PluginInstance(spec));
               this.pluginMap.clear();
               for (let plugin of this.plugins)
                   plugin.update(this);
               this.docView = new DocView(this);
               this.inputState.ensureHandlers(this, this.plugins);
               this.mountStyles();
               this.updateAttrs();
               this.bidiCache = [];
           }
           finally {
               this.updateState = 0 /* UpdateState.Idle */;
           }
           if (hadFocus)
               this.focus();
           this.requestMeasure();
       }
       updatePlugins(update) {
           let prevSpecs = update.startState.facet(viewPlugin), specs = update.state.facet(viewPlugin);
           if (prevSpecs != specs) {
               let newPlugins = [];
               for (let spec of specs) {
                   let found = prevSpecs.indexOf(spec);
                   if (found < 0) {
                       newPlugins.push(new PluginInstance(spec));
                   }
                   else {
                       let plugin = this.plugins[found];
                       plugin.mustUpdate = update;
                       newPlugins.push(plugin);
                   }
               }
               for (let plugin of this.plugins)
                   if (plugin.mustUpdate != update)
                       plugin.destroy(this);
               this.plugins = newPlugins;
               this.pluginMap.clear();
               this.inputState.ensureHandlers(this, this.plugins);
           }
           else {
               for (let p of this.plugins)
                   p.mustUpdate = update;
           }
           for (let i = 0; i < this.plugins.length; i++)
               this.plugins[i].update(this);
       }
       /**
       @internal
       */
       measure(flush = true) {
           if (this.destroyed)
               return;
           if (this.measureScheduled > -1)
               cancelAnimationFrame(this.measureScheduled);
           this.measureScheduled = 0; // Prevent requestMeasure calls from scheduling another animation frame
           if (flush)
               this.observer.forceFlush();
           let updated = null;
           let { scrollHeight, scrollTop, clientHeight } = this.scrollDOM;
           let refHeight = scrollTop > scrollHeight - clientHeight - 4 ? scrollHeight : scrollTop;
           try {
               for (let i = 0;; i++) {
                   this.updateState = 1 /* UpdateState.Measuring */;
                   let oldViewport = this.viewport;
                   let refBlock = this.viewState.lineBlockAtHeight(refHeight);
                   let changed = this.viewState.measure(this);
                   if (!changed && !this.measureRequests.length && this.viewState.scrollTarget == null)
                       break;
                   if (i > 5) {
                       console.warn(this.measureRequests.length
                           ? "Measure loop restarted more than 5 times"
                           : "Viewport failed to stabilize");
                       break;
                   }
                   let measuring = [];
                   // Only run measure requests in this cycle when the viewport didn't change
                   if (!(changed & 4 /* UpdateFlag.Viewport */))
                       [this.measureRequests, measuring] = [measuring, this.measureRequests];
                   let measured = measuring.map(m => {
                       try {
                           return m.read(this);
                       }
                       catch (e) {
                           logException(this.state, e);
                           return BadMeasure;
                       }
                   });
                   let update = ViewUpdate.create(this, this.state, []), redrawn = false, scrolled = false;
                   update.flags |= changed;
                   if (!updated)
                       updated = update;
                   else
                       updated.flags |= changed;
                   this.updateState = 2 /* UpdateState.Updating */;
                   if (!update.empty) {
                       this.updatePlugins(update);
                       this.inputState.update(update);
                       this.updateAttrs();
                       redrawn = this.docView.update(update);
                   }
                   for (let i = 0; i < measuring.length; i++)
                       if (measured[i] != BadMeasure) {
                           try {
                               let m = measuring[i];
                               if (m.write)
                                   m.write(measured[i], this);
                           }
                           catch (e) {
                               logException(this.state, e);
                           }
                       }
                   if (this.viewState.scrollTarget) {
                       this.docView.scrollIntoView(this.viewState.scrollTarget);
                       this.viewState.scrollTarget = null;
                       scrolled = true;
                   }
                   else {
                       let diff = this.viewState.lineBlockAt(refBlock.from).top - refBlock.top;
                       if (diff > 1 || diff < -1) {
                           this.scrollDOM.scrollTop += diff;
                           scrolled = true;
                       }
                   }
                   if (redrawn)
                       this.docView.updateSelection(true);
                   if (this.viewport.from == oldViewport.from && this.viewport.to == oldViewport.to &&
                       !scrolled && this.measureRequests.length == 0)
                       break;
               }
           }
           finally {
               this.updateState = 0 /* UpdateState.Idle */;
               this.measureScheduled = -1;
           }
           if (updated && !updated.empty)
               for (let listener of this.state.facet(updateListener))
                   listener(updated);
       }
       /**
       Get the CSS classes for the currently active editor themes.
       */
       get themeClasses() {
           return baseThemeID + " " +
               (this.state.facet(darkTheme) ? baseDarkID : baseLightID) + " " +
               this.state.facet(theme);
       }
       updateAttrs() {
           let editorAttrs = attrsFromFacet(this, editorAttributes, {
               class: "cm-editor" + (this.hasFocus ? " cm-focused " : " ") + this.themeClasses
           });
           let contentAttrs = {
               spellcheck: "false",
               autocorrect: "off",
               autocapitalize: "off",
               translate: "no",
               contenteditable: !this.state.facet(editable) ? "false" : "true",
               class: "cm-content",
               style: `${browser.tabSize}: ${this.state.tabSize}`,
               role: "textbox",
               "aria-multiline": "true"
           };
           if (this.state.readOnly)
               contentAttrs["aria-readonly"] = "true";
           attrsFromFacet(this, contentAttributes, contentAttrs);
           let changed = this.observer.ignore(() => {
               let changedContent = updateAttrs(this.contentDOM, this.contentAttrs, contentAttrs);
               let changedEditor = updateAttrs(this.dom, this.editorAttrs, editorAttrs);
               return changedContent || changedEditor;
           });
           this.editorAttrs = editorAttrs;
           this.contentAttrs = contentAttrs;
           return changed;
       }
       showAnnouncements(trs) {
           let first = true;
           for (let tr of trs)
               for (let effect of tr.effects)
                   if (effect.is(EditorView.announce)) {
                       if (first)
                           this.announceDOM.textContent = "";
                       first = false;
                       let div = this.announceDOM.appendChild(document.createElement("div"));
                       div.textContent = effect.value;
                   }
       }
       mountStyles() {
           this.styleModules = this.state.facet(styleModule);
           StyleModule.mount(this.root, this.styleModules.concat(baseTheme$1).reverse());
       }
       readMeasured() {
           if (this.updateState == 2 /* UpdateState.Updating */)
               throw new Error("Reading the editor layout isn't allowed during an update");
           if (this.updateState == 0 /* UpdateState.Idle */ && this.measureScheduled > -1)
               this.measure(false);
       }
       /**
       Schedule a layout measurement, optionally providing callbacks to
       do custom DOM measuring followed by a DOM write phase. Using
       this is preferable reading DOM layout directly from, for
       example, an event handler, because it'll make sure measuring and
       drawing done by other components is synchronized, avoiding
       unnecessary DOM layout computations.
       */
       requestMeasure(request) {
           if (this.measureScheduled < 0)
               this.measureScheduled = this.win.requestAnimationFrame(() => this.measure());
           if (request) {
               if (request.key != null)
                   for (let i = 0; i < this.measureRequests.length; i++) {
                       if (this.measureRequests[i].key === request.key) {
                           this.measureRequests[i] = request;
                           return;
                       }
                   }
               this.measureRequests.push(request);
           }
       }
       /**
       Get the value of a specific plugin, if present. Note that
       plugins that crash can be dropped from a view, so even when you
       know you registered a given plugin, it is recommended to check
       the return value of this method.
       */
       plugin(plugin) {
           let known = this.pluginMap.get(plugin);
           if (known === undefined || known && known.spec != plugin)
               this.pluginMap.set(plugin, known = this.plugins.find(p => p.spec == plugin) || null);
           return known && known.update(this).value;
       }
       /**
       The top position of the document, in screen coordinates. This
       may be negative when the editor is scrolled down. Points
       directly to the top of the first line, not above the padding.
       */
       get documentTop() {
           return this.contentDOM.getBoundingClientRect().top + this.viewState.paddingTop;
       }
       /**
       Reports the padding above and below the document.
       */
       get documentPadding() {
           return { top: this.viewState.paddingTop, bottom: this.viewState.paddingBottom };
       }
       /**
       Find the text line or block widget at the given vertical
       position (which is interpreted as relative to the [top of the
       document](https://codemirror.net/6/docs/ref/#view.EditorView.documentTop)).
       */
       elementAtHeight(height) {
           this.readMeasured();
           return this.viewState.elementAtHeight(height);
       }
       /**
       Find the line block (see
       [`lineBlockAt`](https://codemirror.net/6/docs/ref/#view.EditorView.lineBlockAt) at the given
       height, again interpreted relative to the [top of the
       document](https://codemirror.net/6/docs/ref/#view.EditorView.documentTop).
       */
       lineBlockAtHeight(height) {
           this.readMeasured();
           return this.viewState.lineBlockAtHeight(height);
       }
       /**
       Get the extent and vertical position of all [line
       blocks](https://codemirror.net/6/docs/ref/#view.EditorView.lineBlockAt) in the viewport. Positions
       are relative to the [top of the
       document](https://codemirror.net/6/docs/ref/#view.EditorView.documentTop);
       */
       get viewportLineBlocks() {
           return this.viewState.viewportLines;
       }
       /**
       Find the line block around the given document position. A line
       block is a range delimited on both sides by either a
       non-[hidden](https://codemirror.net/6/docs/ref/#view.Decoration^replace) line breaks, or the
       start/end of the document. It will usually just hold a line of
       text, but may be broken into multiple textblocks by block
       widgets.
       */
       lineBlockAt(pos) {
           return this.viewState.lineBlockAt(pos);
       }
       /**
       The editor's total content height.
       */
       get contentHeight() {
           return this.viewState.contentHeight;
       }
       /**
       Move a cursor position by [grapheme
       cluster](https://codemirror.net/6/docs/ref/#state.findClusterBreak). `forward` determines whether
       the motion is away from the line start, or towards it. In
       bidirectional text, the line is traversed in visual order, using
       the editor's [text direction](https://codemirror.net/6/docs/ref/#view.EditorView.textDirection).
       When the start position was the last one on the line, the
       returned position will be across the line break. If there is no
       further line, the original position is returned.
       
       By default, this method moves over a single cluster. The
       optional `by` argument can be used to move across more. It will
       be called with the first cluster as argument, and should return
       a predicate that determines, for each subsequent cluster,
       whether it should also be moved over.
       */
       moveByChar(start, forward, by) {
           return skipAtoms(this, start, moveByChar(this, start, forward, by));
       }
       /**
       Move a cursor position across the next group of either
       [letters](https://codemirror.net/6/docs/ref/#state.EditorState.charCategorizer) or non-letter
       non-whitespace characters.
       */
       moveByGroup(start, forward) {
           return skipAtoms(this, start, moveByChar(this, start, forward, initial => byGroup(this, start.head, initial)));
       }
       /**
       Move to the next line boundary in the given direction. If
       `includeWrap` is true, line wrapping is on, and there is a
       further wrap point on the current line, the wrap point will be
       returned. Otherwise this function will return the start or end
       of the line.
       */
       moveToLineBoundary(start, forward, includeWrap = true) {
           return moveToLineBoundary(this, start, forward, includeWrap);
       }
       /**
       Move a cursor position vertically. When `distance` isn't given,
       it defaults to moving to the next line (including wrapped
       lines). Otherwise, `distance` should provide a positive distance
       in pixels.
       
       When `start` has a
       [`goalColumn`](https://codemirror.net/6/docs/ref/#state.SelectionRange.goalColumn), the vertical
       motion will use that as a target horizontal position. Otherwise,
       the cursor's own horizontal position is used. The returned
       cursor will have its goal column set to whichever column was
       used.
       */
       moveVertically(start, forward, distance) {
           return skipAtoms(this, start, moveVertically(this, start, forward, distance));
       }
       /**
       Find the DOM parent node and offset (child offset if `node` is
       an element, character offset when it is a text node) at the
       given document position.
       
       Note that for positions that aren't currently in
       `visibleRanges`, the resulting DOM position isn't necessarily
       meaningful (it may just point before or after a placeholder
       element).
       */
       domAtPos(pos) {
           return this.docView.domAtPos(pos);
       }
       /**
       Find the document position at the given DOM node. Can be useful
       for associating positions with DOM events. Will raise an error
       when `node` isn't part of the editor content.
       */
       posAtDOM(node, offset = 0) {
           return this.docView.posFromDOM(node, offset);
       }
       posAtCoords(coords, precise = true) {
           this.readMeasured();
           return posAtCoords(this, coords, precise);
       }
       /**
       Get the screen coordinates at the given document position.
       `side` determines whether the coordinates are based on the
       element before (-1) or after (1) the position (if no element is
       available on the given side, the method will transparently use
       another strategy to get reasonable coordinates).
       */
       coordsAtPos(pos, side = 1) {
           this.readMeasured();
           let rect = this.docView.coordsAt(pos, side);
           if (!rect || rect.left == rect.right)
               return rect;
           let line = this.state.doc.lineAt(pos), order = this.bidiSpans(line);
           let span = order[BidiSpan.find(order, pos - line.from, -1, side)];
           return flattenRect(rect, (span.dir == Direction.LTR) == (side > 0));
       }
       /**
       The default width of a character in the editor. May not
       accurately reflect the width of all characters (given variable
       width fonts or styling of invididual ranges).
       */
       get defaultCharacterWidth() { return this.viewState.heightOracle.charWidth; }
       /**
       The default height of a line in the editor. May not be accurate
       for all lines.
       */
       get defaultLineHeight() { return this.viewState.heightOracle.lineHeight; }
       /**
       The text direction
       ([`direction`](https://developer.mozilla.org/en-US/docs/Web/CSS/direction)
       CSS property) of the editor's content element.
       */
       get textDirection() { return this.viewState.defaultTextDirection; }
       /**
       Find the text direction of the block at the given position, as
       assigned by CSS. If
       [`perLineTextDirection`](https://codemirror.net/6/docs/ref/#view.EditorView^perLineTextDirection)
       isn't enabled, or the given position is outside of the viewport,
       this will always return the same as
       [`textDirection`](https://codemirror.net/6/docs/ref/#view.EditorView.textDirection). Note that
       this may trigger a DOM layout.
       */
       textDirectionAt(pos) {
           let perLine = this.state.facet(perLineTextDirection);
           if (!perLine || pos < this.viewport.from || pos > this.viewport.to)
               return this.textDirection;
           this.readMeasured();
           return this.docView.textDirectionAt(pos);
       }
       /**
       Whether this editor [wraps lines](https://codemirror.net/6/docs/ref/#view.EditorView.lineWrapping)
       (as determined by the
       [`white-space`](https://developer.mozilla.org/en-US/docs/Web/CSS/white-space)
       CSS property of its content element).
       */
       get lineWrapping() { return this.viewState.heightOracle.lineWrapping; }
       /**
       Returns the bidirectional text structure of the given line
       (which should be in the current document) as an array of span
       objects. The order of these spans matches the [text
       direction](https://codemirror.net/6/docs/ref/#view.EditorView.textDirection)—if that is
       left-to-right, the leftmost spans come first, otherwise the
       rightmost spans come first.
       */
       bidiSpans(line) {
           if (line.length > MaxBidiLine)
               return trivialOrder(line.length);
           let dir = this.textDirectionAt(line.from);
           for (let entry of this.bidiCache)
               if (entry.from == line.from && entry.dir == dir)
                   return entry.order;
           let order = computeOrder(line.text, dir);
           this.bidiCache.push(new CachedOrder(line.from, line.to, dir, order));
           return order;
       }
       /**
       Check whether the editor has focus.
       */
       get hasFocus() {
           var _a;
           // Safari return false for hasFocus when the context menu is open
           // or closing, which leads us to ignore selection changes from the
           // context menu because it looks like the editor isn't focused.
           // This kludges around that.
           return (this.dom.ownerDocument.hasFocus() || browser.safari && ((_a = this.inputState) === null || _a === void 0 ? void 0 : _a.lastContextMenu) > Date.now() - 3e4) &&
               this.root.activeElement == this.contentDOM;
       }
       /**
       Put focus on the editor.
       */
       focus() {
           this.observer.ignore(() => {
               focusPreventScroll(this.contentDOM);
               this.docView.updateSelection();
           });
       }
       /**
       Update the [root](https://codemirror.net/6/docs/ref/##view.EditorViewConfig.root) in which the editor lives. This is only
       necessary when moving the editor's existing DOM to a new window or shadow root.
       */
       setRoot(root) {
           if (this._root != root) {
               this._root = root;
               this.observer.setWindow((root.nodeType == 9 ? root : root.ownerDocument).defaultView);
               this.mountStyles();
           }
       }
       /**
       Clean up this editor view, removing its element from the
       document, unregistering event handlers, and notifying
       plugins. The view instance can no longer be used after
       calling this.
       */
       destroy() {
           for (let plugin of this.plugins)
               plugin.destroy(this);
           this.plugins = [];
           this.inputState.destroy();
           this.dom.remove();
           this.observer.destroy();
           if (this.measureScheduled > -1)
               cancelAnimationFrame(this.measureScheduled);
           this.destroyed = true;
       }
       /**
       Returns an effect that can be
       [added](https://codemirror.net/6/docs/ref/#state.TransactionSpec.effects) to a transaction to
       cause it to scroll the given position or range into view.
       */
       static scrollIntoView(pos, options = {}) {
           return scrollIntoView.of(new ScrollTarget(typeof pos == "number" ? EditorSelection.cursor(pos) : pos, options.y, options.x, options.yMargin, options.xMargin));
       }
       /**
       Returns an extension that can be used to add DOM event handlers.
       The value should be an object mapping event names to handler
       functions. For any given event, such functions are ordered by
       extension precedence, and the first handler to return true will
       be assumed to have handled that event, and no other handlers or
       built-in behavior will be activated for it. These are registered
       on the [content element](https://codemirror.net/6/docs/ref/#view.EditorView.contentDOM), except
       for `scroll` handlers, which will be called any time the
       editor's [scroll element](https://codemirror.net/6/docs/ref/#view.EditorView.scrollDOM) or one of
       its parent nodes is scrolled.
       */
       static domEventHandlers(handlers) {
           return ViewPlugin.define(() => ({}), { eventHandlers: handlers });
       }
       /**
       Create a theme extension. The first argument can be a
       [`style-mod`](https://github.com/marijnh/style-mod#documentation)
       style spec providing the styles for the theme. These will be
       prefixed with a generated class for the style.
       
       Because the selectors will be prefixed with a scope class, rule
       that directly match the editor's [wrapper
       element](https://codemirror.net/6/docs/ref/#view.EditorView.dom)—to which the scope class will be
       added—need to be explicitly differentiated by adding an `&` to
       the selector for that element—for example
       `&.cm-focused`.
       
       When `dark` is set to true, the theme will be marked as dark,
       which will cause the `&dark` rules from [base
       themes](https://codemirror.net/6/docs/ref/#view.EditorView^baseTheme) to be used (as opposed to
       `&light` when a light theme is active).
       */
       static theme(spec, options) {
           let prefix = StyleModule.newName();
           let result = [theme.of(prefix), styleModule.of(buildTheme(`.${prefix}`, spec))];
           if (options && options.dark)
               result.push(darkTheme.of(true));
           return result;
       }
       /**
       Create an extension that adds styles to the base theme. Like
       with [`theme`](https://codemirror.net/6/docs/ref/#view.EditorView^theme), use `&` to indicate the
       place of the editor wrapper element when directly targeting
       that. You can also use `&dark` or `&light` instead to only
       target editors with a dark or light theme.
       */
       static baseTheme(spec) {
           return Prec.lowest(styleModule.of(buildTheme("." + baseThemeID, spec, lightDarkIDs)));
       }
       /**
       Retrieve an editor view instance from the view's DOM
       representation.
       */
       static findFromDOM(dom) {
           var _a;
           let content = dom.querySelector(".cm-content");
           let cView = content && ContentView.get(content) || ContentView.get(dom);
           return ((_a = cView === null || cView === void 0 ? void 0 : cView.rootView) === null || _a === void 0 ? void 0 : _a.view) || null;
       }
   }
   /**
   Facet to add a [style
   module](https://github.com/marijnh/style-mod#documentation) to
   an editor view. The view will ensure that the module is
   mounted in its [document
   root](https://codemirror.net/6/docs/ref/#view.EditorView.constructor^config.root).
   */
   EditorView.styleModule = styleModule;
   /**
   An input handler can override the way changes to the editable
   DOM content are handled. Handlers are passed the document
   positions between which the change was found, and the new
   content. When one returns true, no further input handlers are
   called and the default behavior is prevented.
   */
   EditorView.inputHandler = inputHandler;
   /**
   By default, the editor assumes all its content has the same
   [text direction](https://codemirror.net/6/docs/ref/#view.Direction). Configure this with a `true`
   value to make it read the text direction of every (rendered)
   line separately.
   */
   EditorView.perLineTextDirection = perLineTextDirection;
   /**
   Allows you to provide a function that should be called when the
   library catches an exception from an extension (mostly from view
   plugins, but may be used by other extensions to route exceptions
   from user-code-provided callbacks). This is mostly useful for
   debugging and logging. See [`logException`](https://codemirror.net/6/docs/ref/#view.logException).
   */
   EditorView.exceptionSink = exceptionSink;
   /**
   A facet that can be used to register a function to be called
   every time the view updates.
   */
   EditorView.updateListener = updateListener;
   /**
   Facet that controls whether the editor content DOM is editable.
   When its highest-precedence value is `false`, the element will
   not have its `contenteditable` attribute set. (Note that this
   doesn't affect API calls that change the editor content, even
   when those are bound to keys or buttons. See the
   [`readOnly`](https://codemirror.net/6/docs/ref/#state.EditorState.readOnly) facet for that.)
   */
   EditorView.editable = editable;
   /**
   Allows you to influence the way mouse selection happens. The
   functions in this facet will be called for a `mousedown` event
   on the editor, and can return an object that overrides the way a
   selection is computed from that mouse click or drag.
   */
   EditorView.mouseSelectionStyle = mouseSelectionStyle;
   /**
   Facet used to configure whether a given selection drag event
   should move or copy the selection. The given predicate will be
   called with the `mousedown` event, and can return `true` when
   the drag should move the content.
   */
   EditorView.dragMovesSelection = dragMovesSelection$1;
   /**
   Facet used to configure whether a given selecting click adds a
   new range to the existing selection or replaces it entirely. The
   default behavior is to check `event.metaKey` on macOS, and
   `event.ctrlKey` elsewhere.
   */
   EditorView.clickAddsSelectionRange = clickAddsSelectionRange;
   /**
   A facet that determines which [decorations](https://codemirror.net/6/docs/ref/#view.Decoration)
   are shown in the view. Decorations can be provided in two
   ways—directly, or via a function that takes an editor view.

   Only decoration sets provided directly are allowed to influence
   the editor's vertical layout structure. The ones provided as
   functions are called _after_ the new viewport has been computed,
   and thus **must not** introduce block widgets or replacing
   decorations that cover line breaks.

   If you want decorated ranges to behave like atomic units for
   cursor motion and deletion purposes, also provide the range set
   containing the decorations to
   [`EditorView.atomicRanges`](https://codemirror.net/6/docs/ref/#view.EditorView^atomicRanges).
   */
   EditorView.decorations = decorations;
   /**
   Used to provide ranges that should be treated as atoms as far as
   cursor motion is concerned. This causes methods like
   [`moveByChar`](https://codemirror.net/6/docs/ref/#view.EditorView.moveByChar) and
   [`moveVertically`](https://codemirror.net/6/docs/ref/#view.EditorView.moveVertically) (and the
   commands built on top of them) to skip across such regions when
   a selection endpoint would enter them. This does _not_ prevent
   direct programmatic [selection
   updates](https://codemirror.net/6/docs/ref/#state.TransactionSpec.selection) from moving into such
   regions.
   */
   EditorView.atomicRanges = atomicRanges;
   /**
   Facet that allows extensions to provide additional scroll
   margins (space around the sides of the scrolling element that
   should be considered invisible). This can be useful when the
   plugin introduces elements that cover part of that element (for
   example a horizontally fixed gutter).
   */
   EditorView.scrollMargins = scrollMargins;
   /**
   This facet records whether a dark theme is active. The extension
   returned by [`theme`](https://codemirror.net/6/docs/ref/#view.EditorView^theme) automatically
   includes an instance of this when the `dark` option is set to
   true.
   */
   EditorView.darkTheme = darkTheme;
   /**
   Facet that provides additional DOM attributes for the editor's
   editable DOM element.
   */
   EditorView.contentAttributes = contentAttributes;
   /**
   Facet that provides DOM attributes for the editor's outer
   element.
   */
   EditorView.editorAttributes = editorAttributes;
   /**
   An extension that enables line wrapping in the editor (by
   setting CSS `white-space` to `pre-wrap` in the content).
   */
   EditorView.lineWrapping = /*@__PURE__*/EditorView.contentAttributes.of({ "class": "cm-lineWrapping" });
   /**
   State effect used to include screen reader announcements in a
   transaction. These will be added to the DOM in a visually hidden
   element with `aria-live="polite"` set, and should be used to
   describe effects that are visually obvious but may not be
   noticed by screen reader users (such as moving to the next
   search match).
   */
   EditorView.announce = /*@__PURE__*/StateEffect.define();
   // Maximum line length for which we compute accurate bidi info
   const MaxBidiLine = 4096;
   const BadMeasure = {};
   class CachedOrder {
       constructor(from, to, dir, order) {
           this.from = from;
           this.to = to;
           this.dir = dir;
           this.order = order;
       }
       static update(cache, changes) {
           if (changes.empty)
               return cache;
           let result = [], lastDir = cache.length ? cache[cache.length - 1].dir : Direction.LTR;
           for (let i = Math.max(0, cache.length - 10); i < cache.length; i++) {
               let entry = cache[i];
               if (entry.dir == lastDir && !changes.touchesRange(entry.from, entry.to))
                   result.push(new CachedOrder(changes.mapPos(entry.from, 1), changes.mapPos(entry.to, -1), entry.dir, entry.order));
           }
           return result;
       }
   }
   function attrsFromFacet(view, facet, base) {
       for (let sources = view.state.facet(facet), i = sources.length - 1; i >= 0; i--) {
           let source = sources[i], value = typeof source == "function" ? source(view) : source;
           if (value)
               combineAttrs(value, base);
       }
       return base;
   }

   const currentPlatform = browser.mac ? "mac" : browser.windows ? "win" : browser.linux ? "linux" : "key";
   function normalizeKeyName(name, platform) {
       const parts = name.split(/-(?!$)/);
       let result = parts[parts.length - 1];
       if (result == "Space")
           result = " ";
       let alt, ctrl, shift, meta;
       for (let i = 0; i < parts.length - 1; ++i) {
           const mod = parts[i];
           if (/^(cmd|meta|m)$/i.test(mod))
               meta = true;
           else if (/^a(lt)?$/i.test(mod))
               alt = true;
           else if (/^(c|ctrl|control)$/i.test(mod))
               ctrl = true;
           else if (/^s(hift)?$/i.test(mod))
               shift = true;
           else if (/^mod$/i.test(mod)) {
               if (platform == "mac")
                   meta = true;
               else
                   ctrl = true;
           }
           else
               throw new Error("Unrecognized modifier name: " + mod);
       }
       if (alt)
           result = "Alt-" + result;
       if (ctrl)
           result = "Ctrl-" + result;
       if (meta)
           result = "Meta-" + result;
       if (shift)
           result = "Shift-" + result;
       return result;
   }
   function modifiers(name, event, shift) {
       if (event.altKey)
           name = "Alt-" + name;
       if (event.ctrlKey)
           name = "Ctrl-" + name;
       if (event.metaKey)
           name = "Meta-" + name;
       if (shift !== false && event.shiftKey)
           name = "Shift-" + name;
       return name;
   }
   const handleKeyEvents = /*@__PURE__*/Prec.default(/*@__PURE__*/EditorView.domEventHandlers({
       keydown(event, view) {
           return runHandlers(getKeymap(view.state), event, view, "editor");
       }
   }));
   /**
   Facet used for registering keymaps.

   You can add multiple keymaps to an editor. Their priorities
   determine their precedence (the ones specified early or with high
   priority get checked first). When a handler has returned `true`
   for a given key, no further handlers are called.
   */
   const keymap = /*@__PURE__*/Facet.define({ enables: handleKeyEvents });
   const Keymaps = /*@__PURE__*/new WeakMap();
   // This is hidden behind an indirection, rather than directly computed
   // by the facet, to keep internal types out of the facet's type.
   function getKeymap(state) {
       let bindings = state.facet(keymap);
       let map = Keymaps.get(bindings);
       if (!map)
           Keymaps.set(bindings, map = buildKeymap(bindings.reduce((a, b) => a.concat(b), [])));
       return map;
   }
   let storedPrefix = null;
   const PrefixTimeout = 4000;
   function buildKeymap(bindings, platform = currentPlatform) {
       let bound = Object.create(null);
       let isPrefix = Object.create(null);
       let checkPrefix = (name, is) => {
           let current = isPrefix[name];
           if (current == null)
               isPrefix[name] = is;
           else if (current != is)
               throw new Error("Key binding " + name + " is used both as a regular binding and as a multi-stroke prefix");
       };
       let add = (scope, key, command, preventDefault) => {
           let scopeObj = bound[scope] || (bound[scope] = Object.create(null));
           let parts = key.split(/ (?!$)/).map(k => normalizeKeyName(k, platform));
           for (let i = 1; i < parts.length; i++) {
               let prefix = parts.slice(0, i).join(" ");
               checkPrefix(prefix, true);
               if (!scopeObj[prefix])
                   scopeObj[prefix] = {
                       preventDefault: true,
                       commands: [(view) => {
                               let ourObj = storedPrefix = { view, prefix, scope };
                               setTimeout(() => { if (storedPrefix == ourObj)
                                   storedPrefix = null; }, PrefixTimeout);
                               return true;
                           }]
                   };
           }
           let full = parts.join(" ");
           checkPrefix(full, false);
           let binding = scopeObj[full] || (scopeObj[full] = { preventDefault: false, commands: [] });
           binding.commands.push(command);
           if (preventDefault)
               binding.preventDefault = true;
       };
       for (let b of bindings) {
           let name = b[platform] || b.key;
           if (!name)
               continue;
           for (let scope of b.scope ? b.scope.split(" ") : ["editor"]) {
               add(scope, name, b.run, b.preventDefault);
               if (b.shift)
                   add(scope, "Shift-" + name, b.shift, b.preventDefault);
           }
       }
       return bound;
   }
   function runHandlers(map, event, view, scope) {
       let name = keyName(event);
       let charCode = codePointAt(name, 0), isChar = codePointSize(charCode) == name.length && name != " ";
       let prefix = "", fallthrough = false;
       if (storedPrefix && storedPrefix.view == view && storedPrefix.scope == scope) {
           prefix = storedPrefix.prefix + " ";
           if (fallthrough = modifierCodes.indexOf(event.keyCode) < 0)
               storedPrefix = null;
       }
       let runFor = (binding) => {
           if (binding) {
               for (let cmd of binding.commands)
                   if (cmd(view))
                       return true;
               if (binding.preventDefault)
                   fallthrough = true;
           }
           return false;
       };
       let scopeObj = map[scope], baseName, shiftName;
       if (scopeObj) {
           if (runFor(scopeObj[prefix + modifiers(name, event, !isChar)]))
               return true;
           if (isChar && (event.shiftKey || event.altKey || event.metaKey || charCode > 127) &&
               (baseName = base[event.keyCode]) && baseName != name) {
               if (runFor(scopeObj[prefix + modifiers(baseName, event, true)]))
                   return true;
               else if (event.shiftKey && (shiftName = shift[event.keyCode]) != name && shiftName != baseName &&
                   runFor(scopeObj[prefix + modifiers(shiftName, event, false)]))
                   return true;
           }
           else if (isChar && event.shiftKey) {
               if (runFor(scopeObj[prefix + modifiers(name, event, true)]))
                   return true;
           }
       }
       return fallthrough;
   }

   const CanHidePrimary = !browser.ios; // FIXME test IE
   const selectionConfig = /*@__PURE__*/Facet.define({
       combine(configs) {
           return combineConfig(configs, {
               cursorBlinkRate: 1200,
               drawRangeCursor: true
           }, {
               cursorBlinkRate: (a, b) => Math.min(a, b),
               drawRangeCursor: (a, b) => a || b
           });
       }
   });
   /**
   Returns an extension that hides the browser's native selection and
   cursor, replacing the selection with a background behind the text
   (with the `cm-selectionBackground` class), and the
   cursors with elements overlaid over the code (using
   `cm-cursor-primary` and `cm-cursor-secondary`).

   This allows the editor to display secondary selection ranges, and
   tends to produce a type of selection more in line with that users
   expect in a text editor (the native selection styling will often
   leave gaps between lines and won't fill the horizontal space after
   a line when the selection continues past it).

   It does have a performance cost, in that it requires an extra DOM
   layout cycle for many updates (the selection is drawn based on DOM
   layout information that's only available after laying out the
   content).
   */
   function drawSelection(config = {}) {
       return [
           selectionConfig.of(config),
           drawSelectionPlugin,
           hideNativeSelection
       ];
   }
   class Piece {
       constructor(left, top, width, height, className) {
           this.left = left;
           this.top = top;
           this.width = width;
           this.height = height;
           this.className = className;
       }
       draw() {
           let elt = document.createElement("div");
           elt.className = this.className;
           this.adjust(elt);
           return elt;
       }
       adjust(elt) {
           elt.style.left = this.left + "px";
           elt.style.top = this.top + "px";
           if (this.width >= 0)
               elt.style.width = this.width + "px";
           elt.style.height = this.height + "px";
       }
       eq(p) {
           return this.left == p.left && this.top == p.top && this.width == p.width && this.height == p.height &&
               this.className == p.className;
       }
   }
   const drawSelectionPlugin = /*@__PURE__*/ViewPlugin.fromClass(class {
       constructor(view) {
           this.view = view;
           this.rangePieces = [];
           this.cursors = [];
           this.measureReq = { read: this.readPos.bind(this), write: this.drawSel.bind(this) };
           this.selectionLayer = view.scrollDOM.appendChild(document.createElement("div"));
           this.selectionLayer.className = "cm-selectionLayer";
           this.selectionLayer.setAttribute("aria-hidden", "true");
           this.cursorLayer = view.scrollDOM.appendChild(document.createElement("div"));
           this.cursorLayer.className = "cm-cursorLayer";
           this.cursorLayer.setAttribute("aria-hidden", "true");
           view.requestMeasure(this.measureReq);
           this.setBlinkRate();
       }
       setBlinkRate() {
           this.cursorLayer.style.animationDuration = this.view.state.facet(selectionConfig).cursorBlinkRate + "ms";
       }
       update(update) {
           let confChanged = update.startState.facet(selectionConfig) != update.state.facet(selectionConfig);
           if (confChanged || update.selectionSet || update.geometryChanged || update.viewportChanged)
               this.view.requestMeasure(this.measureReq);
           if (update.transactions.some(tr => tr.scrollIntoView))
               this.cursorLayer.style.animationName = this.cursorLayer.style.animationName == "cm-blink" ? "cm-blink2" : "cm-blink";
           if (confChanged)
               this.setBlinkRate();
       }
       readPos() {
           let { state } = this.view, conf = state.facet(selectionConfig);
           let rangePieces = state.selection.ranges.map(r => r.empty ? [] : measureRange(this.view, r)).reduce((a, b) => a.concat(b));
           let cursors = [];
           for (let r of state.selection.ranges) {
               let prim = r == state.selection.main;
               if (r.empty ? !prim || CanHidePrimary : conf.drawRangeCursor) {
                   let piece = measureCursor(this.view, r, prim);
                   if (piece)
                       cursors.push(piece);
               }
           }
           return { rangePieces, cursors };
       }
       drawSel({ rangePieces, cursors }) {
           if (rangePieces.length != this.rangePieces.length || rangePieces.some((p, i) => !p.eq(this.rangePieces[i]))) {
               this.selectionLayer.textContent = "";
               for (let p of rangePieces)
                   this.selectionLayer.appendChild(p.draw());
               this.rangePieces = rangePieces;
           }
           if (cursors.length != this.cursors.length || cursors.some((c, i) => !c.eq(this.cursors[i]))) {
               let oldCursors = this.cursorLayer.children;
               if (oldCursors.length !== cursors.length) {
                   this.cursorLayer.textContent = "";
                   for (const c of cursors)
                       this.cursorLayer.appendChild(c.draw());
               }
               else {
                   cursors.forEach((c, idx) => c.adjust(oldCursors[idx]));
               }
               this.cursors = cursors;
           }
       }
       destroy() {
           this.selectionLayer.remove();
           this.cursorLayer.remove();
       }
   });
   const themeSpec = {
       ".cm-line": {
           "& ::selection": { backgroundColor: "transparent !important" },
           "&::selection": { backgroundColor: "transparent !important" }
       }
   };
   if (CanHidePrimary)
       themeSpec[".cm-line"].caretColor = "transparent !important";
   const hideNativeSelection = /*@__PURE__*/Prec.highest(/*@__PURE__*/EditorView.theme(themeSpec));
   function getBase(view) {
       let rect = view.scrollDOM.getBoundingClientRect();
       let left = view.textDirection == Direction.LTR ? rect.left : rect.right - view.scrollDOM.clientWidth;
       return { left: left - view.scrollDOM.scrollLeft, top: rect.top - view.scrollDOM.scrollTop };
   }
   function wrappedLine(view, pos, inside) {
       let range = EditorSelection.cursor(pos);
       return { from: Math.max(inside.from, view.moveToLineBoundary(range, false, true).from),
           to: Math.min(inside.to, view.moveToLineBoundary(range, true, true).from),
           type: BlockType.Text };
   }
   function blockAt(view, pos) {
       let line = view.lineBlockAt(pos);
       if (Array.isArray(line.type))
           for (let l of line.type) {
               if (l.to > pos || l.to == pos && (l.to == line.to || l.type == BlockType.Text))
                   return l;
           }
       return line;
   }
   function measureRange(view, range) {
       if (range.to <= view.viewport.from || range.from >= view.viewport.to)
           return [];
       let from = Math.max(range.from, view.viewport.from), to = Math.min(range.to, view.viewport.to);
       let ltr = view.textDirection == Direction.LTR;
       let content = view.contentDOM, contentRect = content.getBoundingClientRect(), base = getBase(view);
       let lineStyle = window.getComputedStyle(content.firstChild);
       let leftSide = contentRect.left + parseInt(lineStyle.paddingLeft) + Math.min(0, parseInt(lineStyle.textIndent));
       let rightSide = contentRect.right - parseInt(lineStyle.paddingRight);
       let startBlock = blockAt(view, from), endBlock = blockAt(view, to);
       let visualStart = startBlock.type == BlockType.Text ? startBlock : null;
       let visualEnd = endBlock.type == BlockType.Text ? endBlock : null;
       if (view.lineWrapping) {
           if (visualStart)
               visualStart = wrappedLine(view, from, visualStart);
           if (visualEnd)
               visualEnd = wrappedLine(view, to, visualEnd);
       }
       if (visualStart && visualEnd && visualStart.from == visualEnd.from) {
           return pieces(drawForLine(range.from, range.to, visualStart));
       }
       else {
           let top = visualStart ? drawForLine(range.from, null, visualStart) : drawForWidget(startBlock, false);
           let bottom = visualEnd ? drawForLine(null, range.to, visualEnd) : drawForWidget(endBlock, true);
           let between = [];
           if ((visualStart || startBlock).to < (visualEnd || endBlock).from - 1)
               between.push(piece(leftSide, top.bottom, rightSide, bottom.top));
           else if (top.bottom < bottom.top && view.elementAtHeight((top.bottom + bottom.top) / 2).type == BlockType.Text)
               top.bottom = bottom.top = (top.bottom + bottom.top) / 2;
           return pieces(top).concat(between).concat(pieces(bottom));
       }
       function piece(left, top, right, bottom) {
           return new Piece(left - base.left, top - base.top - 0.01 /* C.Epsilon */, right - left, bottom - top + 0.01 /* C.Epsilon */, "cm-selectionBackground");
       }
       function pieces({ top, bottom, horizontal }) {
           let pieces = [];
           for (let i = 0; i < horizontal.length; i += 2)
               pieces.push(piece(horizontal[i], top, horizontal[i + 1], bottom));
           return pieces;
       }
       // Gets passed from/to in line-local positions
       function drawForLine(from, to, line) {
           let top = 1e9, bottom = -1e9, horizontal = [];
           function addSpan(from, fromOpen, to, toOpen, dir) {
               // Passing 2/-2 is a kludge to force the view to return
               // coordinates on the proper side of block widgets, since
               // normalizing the side there, though appropriate for most
               // coordsAtPos queries, would break selection drawing.
               let fromCoords = view.coordsAtPos(from, (from == line.to ? -2 : 2));
               let toCoords = view.coordsAtPos(to, (to == line.from ? 2 : -2));
               top = Math.min(fromCoords.top, toCoords.top, top);
               bottom = Math.max(fromCoords.bottom, toCoords.bottom, bottom);
               if (dir == Direction.LTR)
                   horizontal.push(ltr && fromOpen ? leftSide : fromCoords.left, ltr && toOpen ? rightSide : toCoords.right);
               else
                   horizontal.push(!ltr && toOpen ? leftSide : toCoords.left, !ltr && fromOpen ? rightSide : fromCoords.right);
           }
           let start = from !== null && from !== void 0 ? from : line.from, end = to !== null && to !== void 0 ? to : line.to;
           // Split the range by visible range and document line
           for (let r of view.visibleRanges)
               if (r.to > start && r.from < end) {
                   for (let pos = Math.max(r.from, start), endPos = Math.min(r.to, end);;) {
                       let docLine = view.state.doc.lineAt(pos);
                       for (let span of view.bidiSpans(docLine)) {
                           let spanFrom = span.from + docLine.from, spanTo = span.to + docLine.from;
                           if (spanFrom >= endPos)
                               break;
                           if (spanTo > pos)
                               addSpan(Math.max(spanFrom, pos), from == null && spanFrom <= start, Math.min(spanTo, endPos), to == null && spanTo >= end, span.dir);
                       }
                       pos = docLine.to + 1;
                       if (pos >= endPos)
                           break;
                   }
               }
           if (horizontal.length == 0)
               addSpan(start, from == null, end, to == null, view.textDirection);
           return { top, bottom, horizontal };
       }
       function drawForWidget(block, top) {
           let y = contentRect.top + (top ? block.top : block.bottom);
           return { top: y, bottom: y, horizontal: [] };
       }
   }
   function measureCursor(view, cursor, primary) {
       let pos = view.coordsAtPos(cursor.head, cursor.assoc || 1);
       if (!pos)
           return null;
       let base = getBase(view);
       return new Piece(pos.left - base.left, pos.top - base.top, -1, pos.bottom - pos.top, primary ? "cm-cursor cm-cursor-primary" : "cm-cursor cm-cursor-secondary");
   }

   function iterMatches(doc, re, from, to, f) {
       re.lastIndex = 0;
       for (let cursor = doc.iterRange(from, to), pos = from, m; !cursor.next().done; pos += cursor.value.length) {
           if (!cursor.lineBreak)
               while (m = re.exec(cursor.value))
                   f(pos + m.index, m);
       }
   }
   function matchRanges(view, maxLength) {
       let visible = view.visibleRanges;
       if (visible.length == 1 && visible[0].from == view.viewport.from &&
           visible[0].to == view.viewport.to)
           return visible;
       let result = [];
       for (let { from, to } of visible) {
           from = Math.max(view.state.doc.lineAt(from).from, from - maxLength);
           to = Math.min(view.state.doc.lineAt(to).to, to + maxLength);
           if (result.length && result[result.length - 1].to >= from)
               result[result.length - 1].to = to;
           else
               result.push({ from, to });
       }
       return result;
   }
   /**
   Helper class used to make it easier to maintain decorations on
   visible code that matches a given regular expression. To be used
   in a [view plugin](https://codemirror.net/6/docs/ref/#view.ViewPlugin). Instances of this object
   represent a matching configuration.
   */
   class MatchDecorator {
       /**
       Create a decorator.
       */
       constructor(config) {
           const { regexp, decoration, decorate, boundary, maxLength = 1000 } = config;
           if (!regexp.global)
               throw new RangeError("The regular expression given to MatchDecorator should have its 'g' flag set");
           this.regexp = regexp;
           if (decorate) {
               this.addMatch = (match, view, from, add) => decorate(add, from, from + match[0].length, match, view);
           }
           else if (decoration) {
               let getDeco = typeof decoration == "function" ? decoration : () => decoration;
               this.addMatch = (match, view, from, add) => add(from, from + match[0].length, getDeco(match, view, from));
           }
           else {
               throw new RangeError("Either 'decorate' or 'decoration' should be provided to MatchDecorator");
           }
           this.boundary = boundary;
           this.maxLength = maxLength;
       }
       /**
       Compute the full set of decorations for matches in the given
       view's viewport. You'll want to call this when initializing your
       plugin.
       */
       createDeco(view) {
           let build = new RangeSetBuilder(), add = build.add.bind(build);
           for (let { from, to } of matchRanges(view, this.maxLength))
               iterMatches(view.state.doc, this.regexp, from, to, (from, m) => this.addMatch(m, view, from, add));
           return build.finish();
       }
       /**
       Update a set of decorations for a view update. `deco` _must_ be
       the set of decorations produced by _this_ `MatchDecorator` for
       the view state before the update.
       */
       updateDeco(update, deco) {
           let changeFrom = 1e9, changeTo = -1;
           if (update.docChanged)
               update.changes.iterChanges((_f, _t, from, to) => {
                   if (to > update.view.viewport.from && from < update.view.viewport.to) {
                       changeFrom = Math.min(from, changeFrom);
                       changeTo = Math.max(to, changeTo);
                   }
               });
           if (update.viewportChanged || changeTo - changeFrom > 1000)
               return this.createDeco(update.view);
           if (changeTo > -1)
               return this.updateRange(update.view, deco.map(update.changes), changeFrom, changeTo);
           return deco;
       }
       updateRange(view, deco, updateFrom, updateTo) {
           for (let r of view.visibleRanges) {
               let from = Math.max(r.from, updateFrom), to = Math.min(r.to, updateTo);
               if (to > from) {
                   let fromLine = view.state.doc.lineAt(from), toLine = fromLine.to < to ? view.state.doc.lineAt(to) : fromLine;
                   let start = Math.max(r.from, fromLine.from), end = Math.min(r.to, toLine.to);
                   if (this.boundary) {
                       for (; from > fromLine.from; from--)
                           if (this.boundary.test(fromLine.text[from - 1 - fromLine.from])) {
                               start = from;
                               break;
                           }
                       for (; to < toLine.to; to++)
                           if (this.boundary.test(toLine.text[to - toLine.from])) {
                               end = to;
                               break;
                           }
                   }
                   let ranges = [], m;
                   let add = (from, to, deco) => ranges.push(deco.range(from, to));
                   if (fromLine == toLine) {
                       this.regexp.lastIndex = start - fromLine.from;
                       while ((m = this.regexp.exec(fromLine.text)) && m.index < end - fromLine.from)
                           this.addMatch(m, view, m.index + fromLine.from, add);
                   }
                   else {
                       iterMatches(view.state.doc, this.regexp, start, end, (from, m) => this.addMatch(m, view, from, add));
                   }
                   deco = deco.update({ filterFrom: start, filterTo: end, filter: (from, to) => from < start || to > end, add: ranges });
               }
           }
           return deco;
       }
   }

   const UnicodeRegexpSupport = /x/.unicode != null ? "gu" : "g";
   const Specials = /*@__PURE__*/new RegExp("[\u0000-\u0008\u000a-\u001f\u007f-\u009f\u00ad\u061c\u200b\u200e\u200f\u2028\u2029\u202d\u202e\u2066\u2067\u2069\ufeff\ufff9-\ufffc]", UnicodeRegexpSupport);
   const Names = {
       0: "null",
       7: "bell",
       8: "backspace",
       10: "newline",
       11: "vertical tab",
       13: "carriage return",
       27: "escape",
       8203: "zero width space",
       8204: "zero width non-joiner",
       8205: "zero width joiner",
       8206: "left-to-right mark",
       8207: "right-to-left mark",
       8232: "line separator",
       8237: "left-to-right override",
       8238: "right-to-left override",
       8294: "left-to-right isolate",
       8295: "right-to-left isolate",
       8297: "pop directional isolate",
       8233: "paragraph separator",
       65279: "zero width no-break space",
       65532: "object replacement"
   };
   let _supportsTabSize = null;
   function supportsTabSize() {
       var _a;
       if (_supportsTabSize == null && typeof document != "undefined" && document.body) {
           let styles = document.body.style;
           _supportsTabSize = ((_a = styles.tabSize) !== null && _a !== void 0 ? _a : styles.MozTabSize) != null;
       }
       return _supportsTabSize || false;
   }
   const specialCharConfig = /*@__PURE__*/Facet.define({
       combine(configs) {
           let config = combineConfig(configs, {
               render: null,
               specialChars: Specials,
               addSpecialChars: null
           });
           if (config.replaceTabs = !supportsTabSize())
               config.specialChars = new RegExp("\t|" + config.specialChars.source, UnicodeRegexpSupport);
           if (config.addSpecialChars)
               config.specialChars = new RegExp(config.specialChars.source + "|" + config.addSpecialChars.source, UnicodeRegexpSupport);
           return config;
       }
   });
   /**
   Returns an extension that installs highlighting of special
   characters.
   */
   function highlightSpecialChars(
   /**
   Configuration options.
   */
   config = {}) {
       return [specialCharConfig.of(config), specialCharPlugin()];
   }
   let _plugin = null;
   function specialCharPlugin() {
       return _plugin || (_plugin = ViewPlugin.fromClass(class {
           constructor(view) {
               this.view = view;
               this.decorations = Decoration.none;
               this.decorationCache = Object.create(null);
               this.decorator = this.makeDecorator(view.state.facet(specialCharConfig));
               this.decorations = this.decorator.createDeco(view);
           }
           makeDecorator(conf) {
               return new MatchDecorator({
                   regexp: conf.specialChars,
                   decoration: (m, view, pos) => {
                       let { doc } = view.state;
                       let code = codePointAt(m[0], 0);
                       if (code == 9) {
                           let line = doc.lineAt(pos);
                           let size = view.state.tabSize, col = countColumn(line.text, size, pos - line.from);
                           return Decoration.replace({ widget: new TabWidget((size - (col % size)) * this.view.defaultCharacterWidth) });
                       }
                       return this.decorationCache[code] ||
                           (this.decorationCache[code] = Decoration.replace({ widget: new SpecialCharWidget(conf, code) }));
                   },
                   boundary: conf.replaceTabs ? undefined : /[^]/
               });
           }
           update(update) {
               let conf = update.state.facet(specialCharConfig);
               if (update.startState.facet(specialCharConfig) != conf) {
                   this.decorator = this.makeDecorator(conf);
                   this.decorations = this.decorator.createDeco(update.view);
               }
               else {
                   this.decorations = this.decorator.updateDeco(update, this.decorations);
               }
           }
       }, {
           decorations: v => v.decorations
       }));
   }
   const DefaultPlaceholder = "\u2022";
   // Assigns placeholder characters from the Control Pictures block to
   // ASCII control characters
   function placeholder$1(code) {
       if (code >= 32)
           return DefaultPlaceholder;
       if (code == 10)
           return "\u2424";
       return String.fromCharCode(9216 + code);
   }
   class SpecialCharWidget extends WidgetType {
       constructor(options, code) {
           super();
           this.options = options;
           this.code = code;
       }
       eq(other) { return other.code == this.code; }
       toDOM(view) {
           let ph = placeholder$1(this.code);
           let desc = view.state.phrase("Control character") + " " + (Names[this.code] || "0x" + this.code.toString(16));
           let custom = this.options.render && this.options.render(this.code, desc, ph);
           if (custom)
               return custom;
           let span = document.createElement("span");
           span.textContent = ph;
           span.title = desc;
           span.setAttribute("aria-label", desc);
           span.className = "cm-specialChar";
           return span;
       }
       ignoreEvent() { return false; }
   }
   class TabWidget extends WidgetType {
       constructor(width) {
           super();
           this.width = width;
       }
       eq(other) { return other.width == this.width; }
       toDOM() {
           let span = document.createElement("span");
           span.textContent = "\t";
           span.className = "cm-tab";
           span.style.width = this.width + "px";
           return span;
       }
       ignoreEvent() { return false; }
   }

   /**
   A gutter marker represents a bit of information attached to a line
   in a specific gutter. Your own custom markers have to extend this
   class.
   */
   class GutterMarker extends RangeValue {
       /**
       @internal
       */
       compare(other) {
           return this == other || this.constructor == other.constructor && this.eq(other);
       }
       /**
       Compare this marker to another marker of the same type.
       */
       eq(other) { return false; }
       /**
       Called if the marker has a `toDOM` method and its representation
       was removed from a gutter.
       */
       destroy(dom) { }
   }
   GutterMarker.prototype.elementClass = "";
   GutterMarker.prototype.toDOM = undefined;
   GutterMarker.prototype.mapMode = MapMode.TrackBefore;
   GutterMarker.prototype.startSide = GutterMarker.prototype.endSide = -1;
   GutterMarker.prototype.point = true;

   // FIXME profile adding a per-Tree TreeNode cache, validating it by
   // parent pointer
   /// The default maximum length of a `TreeBuffer` node.
   const DefaultBufferLength = 1024;
   let nextPropID = 0;
   class Range {
       constructor(from, to) {
           this.from = from;
           this.to = to;
       }
   }
   /// Each [node type](#common.NodeType) or [individual tree](#common.Tree)
   /// can have metadata associated with it in props. Instances of this
   /// class represent prop names.
   class NodeProp {
       /// Create a new node prop type.
       constructor(config = {}) {
           this.id = nextPropID++;
           this.perNode = !!config.perNode;
           this.deserialize = config.deserialize || (() => {
               throw new Error("This node type doesn't define a deserialize function");
           });
       }
       /// This is meant to be used with
       /// [`NodeSet.extend`](#common.NodeSet.extend) or
       /// [`LRParser.configure`](#lr.ParserConfig.props) to compute
       /// prop values for each node type in the set. Takes a [match
       /// object](#common.NodeType^match) or function that returns undefined
       /// if the node type doesn't get this prop, and the prop's value if
       /// it does.
       add(match) {
           if (this.perNode)
               throw new RangeError("Can't add per-node props to node types");
           if (typeof match != "function")
               match = NodeType.match(match);
           return (type) => {
               let result = match(type);
               return result === undefined ? null : [this, result];
           };
       }
   }
   /// Prop that is used to describe matching delimiters. For opening
   /// delimiters, this holds an array of node names (written as a
   /// space-separated string when declaring this prop in a grammar)
   /// for the node types of closing delimiters that match it.
   NodeProp.closedBy = new NodeProp({ deserialize: str => str.split(" ") });
   /// The inverse of [`closedBy`](#common.NodeProp^closedBy). This is
   /// attached to closing delimiters, holding an array of node names
   /// of types of matching opening delimiters.
   NodeProp.openedBy = new NodeProp({ deserialize: str => str.split(" ") });
   /// Used to assign node types to groups (for example, all node
   /// types that represent an expression could be tagged with an
   /// `"Expression"` group).
   NodeProp.group = new NodeProp({ deserialize: str => str.split(" ") });
   /// The hash of the [context](#lr.ContextTracker.constructor)
   /// that the node was parsed in, if any. Used to limit reuse of
   /// contextual nodes.
   NodeProp.contextHash = new NodeProp({ perNode: true });
   /// The distance beyond the end of the node that the tokenizer
   /// looked ahead for any of the tokens inside the node. (The LR
   /// parser only stores this when it is larger than 25, for
   /// efficiency reasons.)
   NodeProp.lookAhead = new NodeProp({ perNode: true });
   /// This per-node prop is used to replace a given node, or part of a
   /// node, with another tree. This is useful to include trees from
   /// different languages in mixed-language parsers.
   NodeProp.mounted = new NodeProp({ perNode: true });
   const noProps = Object.create(null);
   /// Each node in a syntax tree has a node type associated with it.
   class NodeType {
       /// @internal
       constructor(
       /// The name of the node type. Not necessarily unique, but if the
       /// grammar was written properly, different node types with the
       /// same name within a node set should play the same semantic
       /// role.
       name, 
       /// @internal
       props, 
       /// The id of this node in its set. Corresponds to the term ids
       /// used in the parser.
       id, 
       /// @internal
       flags = 0) {
           this.name = name;
           this.props = props;
           this.id = id;
           this.flags = flags;
       }
       /// Define a node type.
       static define(spec) {
           let props = spec.props && spec.props.length ? Object.create(null) : noProps;
           let flags = (spec.top ? 1 /* Top */ : 0) | (spec.skipped ? 2 /* Skipped */ : 0) |
               (spec.error ? 4 /* Error */ : 0) | (spec.name == null ? 8 /* Anonymous */ : 0);
           let type = new NodeType(spec.name || "", props, spec.id, flags);
           if (spec.props)
               for (let src of spec.props) {
                   if (!Array.isArray(src))
                       src = src(type);
                   if (src) {
                       if (src[0].perNode)
                           throw new RangeError("Can't store a per-node prop on a node type");
                       props[src[0].id] = src[1];
                   }
               }
           return type;
       }
       /// Retrieves a node prop for this type. Will return `undefined` if
       /// the prop isn't present on this node.
       prop(prop) { return this.props[prop.id]; }
       /// True when this is the top node of a grammar.
       get isTop() { return (this.flags & 1 /* Top */) > 0; }
       /// True when this node is produced by a skip rule.
       get isSkipped() { return (this.flags & 2 /* Skipped */) > 0; }
       /// Indicates whether this is an error node.
       get isError() { return (this.flags & 4 /* Error */) > 0; }
       /// When true, this node type doesn't correspond to a user-declared
       /// named node, for example because it is used to cache repetition.
       get isAnonymous() { return (this.flags & 8 /* Anonymous */) > 0; }
       /// Returns true when this node's name or one of its
       /// [groups](#common.NodeProp^group) matches the given string.
       is(name) {
           if (typeof name == 'string') {
               if (this.name == name)
                   return true;
               let group = this.prop(NodeProp.group);
               return group ? group.indexOf(name) > -1 : false;
           }
           return this.id == name;
       }
       /// Create a function from node types to arbitrary values by
       /// specifying an object whose property names are node or
       /// [group](#common.NodeProp^group) names. Often useful with
       /// [`NodeProp.add`](#common.NodeProp.add). You can put multiple
       /// names, separated by spaces, in a single property name to map
       /// multiple node names to a single value.
       static match(map) {
           let direct = Object.create(null);
           for (let prop in map)
               for (let name of prop.split(" "))
                   direct[name] = map[prop];
           return (node) => {
               for (let groups = node.prop(NodeProp.group), i = -1; i < (groups ? groups.length : 0); i++) {
                   let found = direct[i < 0 ? node.name : groups[i]];
                   if (found)
                       return found;
               }
           };
       }
   }
   /// An empty dummy node type to use when no actual type is available.
   NodeType.none = new NodeType("", Object.create(null), 0, 8 /* Anonymous */);
   /// A node set holds a collection of node types. It is used to
   /// compactly represent trees by storing their type ids, rather than a
   /// full pointer to the type object, in a numeric array. Each parser
   /// [has](#lr.LRParser.nodeSet) a node set, and [tree
   /// buffers](#common.TreeBuffer) can only store collections of nodes
   /// from the same set. A set can have a maximum of 2**16 (65536) node
   /// types in it, so that the ids fit into 16-bit typed array slots.
   class NodeSet {
       /// Create a set with the given types. The `id` property of each
       /// type should correspond to its position within the array.
       constructor(
       /// The node types in this set, by id.
       types) {
           this.types = types;
           for (let i = 0; i < types.length; i++)
               if (types[i].id != i)
                   throw new RangeError("Node type ids should correspond to array positions when creating a node set");
       }
       /// Create a copy of this set with some node properties added. The
       /// arguments to this method can be created with
       /// [`NodeProp.add`](#common.NodeProp.add).
       extend(...props) {
           let newTypes = [];
           for (let type of this.types) {
               let newProps = null;
               for (let source of props) {
                   let add = source(type);
                   if (add) {
                       if (!newProps)
                           newProps = Object.assign({}, type.props);
                       newProps[add[0].id] = add[1];
                   }
               }
               newTypes.push(newProps ? new NodeType(type.name, newProps, type.id, type.flags) : type);
           }
           return new NodeSet(newTypes);
       }
   }
   const CachedNode = new WeakMap(), CachedInnerNode = new WeakMap();
   /// Options that control iteration. Can be combined with the `|`
   /// operator to enable multiple ones.
   var IterMode;
   (function (IterMode) {
       /// When enabled, iteration will only visit [`Tree`](#common.Tree)
       /// objects, not nodes packed into
       /// [`TreeBuffer`](#common.TreeBuffer)s.
       IterMode[IterMode["ExcludeBuffers"] = 1] = "ExcludeBuffers";
       /// Enable this to make iteration include anonymous nodes (such as
       /// the nodes that wrap repeated grammar constructs into a balanced
       /// tree).
       IterMode[IterMode["IncludeAnonymous"] = 2] = "IncludeAnonymous";
       /// By default, regular [mounted](#common.NodeProp^mounted) nodes
       /// replace their base node in iteration. Enable this to ignore them
       /// instead.
       IterMode[IterMode["IgnoreMounts"] = 4] = "IgnoreMounts";
       /// This option only applies in
       /// [`enter`](#common.SyntaxNode.enter)-style methods. It tells the
       /// library to not enter mounted overlays if one covers the given
       /// position.
       IterMode[IterMode["IgnoreOverlays"] = 8] = "IgnoreOverlays";
   })(IterMode || (IterMode = {}));
   /// A piece of syntax tree. There are two ways to approach these
   /// trees: the way they are actually stored in memory, and the
   /// convenient way.
   ///
   /// Syntax trees are stored as a tree of `Tree` and `TreeBuffer`
   /// objects. By packing detail information into `TreeBuffer` leaf
   /// nodes, the representation is made a lot more memory-efficient.
   ///
   /// However, when you want to actually work with tree nodes, this
   /// representation is very awkward, so most client code will want to
   /// use the [`TreeCursor`](#common.TreeCursor) or
   /// [`SyntaxNode`](#common.SyntaxNode) interface instead, which provides
   /// a view on some part of this data structure, and can be used to
   /// move around to adjacent nodes.
   class Tree {
       /// Construct a new tree. See also [`Tree.build`](#common.Tree^build).
       constructor(
       /// The type of the top node.
       type, 
       /// This node's child nodes.
       children, 
       /// The positions (offsets relative to the start of this tree) of
       /// the children.
       positions, 
       /// The total length of this tree
       length, 
       /// Per-node [node props](#common.NodeProp) to associate with this node.
       props) {
           this.type = type;
           this.children = children;
           this.positions = positions;
           this.length = length;
           /// @internal
           this.props = null;
           if (props && props.length) {
               this.props = Object.create(null);
               for (let [prop, value] of props)
                   this.props[typeof prop == "number" ? prop : prop.id] = value;
           }
       }
       /// @internal
       toString() {
           let mounted = this.prop(NodeProp.mounted);
           if (mounted && !mounted.overlay)
               return mounted.tree.toString();
           let children = "";
           for (let ch of this.children) {
               let str = ch.toString();
               if (str) {
                   if (children)
                       children += ",";
                   children += str;
               }
           }
           return !this.type.name ? children :
               (/\W/.test(this.type.name) && !this.type.isError ? JSON.stringify(this.type.name) : this.type.name) +
                   (children.length ? "(" + children + ")" : "");
       }
       /// Get a [tree cursor](#common.TreeCursor) positioned at the top of
       /// the tree. Mode can be used to [control](#common.IterMode) which
       /// nodes the cursor visits.
       cursor(mode = 0) {
           return new TreeCursor(this.topNode, mode);
       }
       /// Get a [tree cursor](#common.TreeCursor) pointing into this tree
       /// at the given position and side (see
       /// [`moveTo`](#common.TreeCursor.moveTo).
       cursorAt(pos, side = 0, mode = 0) {
           let scope = CachedNode.get(this) || this.topNode;
           let cursor = new TreeCursor(scope);
           cursor.moveTo(pos, side);
           CachedNode.set(this, cursor._tree);
           return cursor;
       }
       /// Get a [syntax node](#common.SyntaxNode) object for the top of the
       /// tree.
       get topNode() {
           return new TreeNode(this, 0, 0, null);
       }
       /// Get the [syntax node](#common.SyntaxNode) at the given position.
       /// If `side` is -1, this will move into nodes that end at the
       /// position. If 1, it'll move into nodes that start at the
       /// position. With 0, it'll only enter nodes that cover the position
       /// from both sides.
       ///
       /// Note that this will not enter
       /// [overlays](#common.MountedTree.overlay), and you often want
       /// [`resolveInner`](#common.Tree.resolveInner) instead.
       resolve(pos, side = 0) {
           let node = resolveNode(CachedNode.get(this) || this.topNode, pos, side, false);
           CachedNode.set(this, node);
           return node;
       }
       /// Like [`resolve`](#common.Tree.resolve), but will enter
       /// [overlaid](#common.MountedTree.overlay) nodes, producing a syntax node
       /// pointing into the innermost overlaid tree at the given position
       /// (with parent links going through all parent structure, including
       /// the host trees).
       resolveInner(pos, side = 0) {
           let node = resolveNode(CachedInnerNode.get(this) || this.topNode, pos, side, true);
           CachedInnerNode.set(this, node);
           return node;
       }
       /// Iterate over the tree and its children, calling `enter` for any
       /// node that touches the `from`/`to` region (if given) before
       /// running over such a node's children, and `leave` (if given) when
       /// leaving the node. When `enter` returns `false`, that node will
       /// not have its children iterated over (or `leave` called).
       iterate(spec) {
           let { enter, leave, from = 0, to = this.length } = spec;
           for (let c = this.cursor((spec.mode || 0) | IterMode.IncludeAnonymous);;) {
               let entered = false;
               if (c.from <= to && c.to >= from && (c.type.isAnonymous || enter(c) !== false)) {
                   if (c.firstChild())
                       continue;
                   entered = true;
               }
               for (;;) {
                   if (entered && leave && !c.type.isAnonymous)
                       leave(c);
                   if (c.nextSibling())
                       break;
                   if (!c.parent())
                       return;
                   entered = true;
               }
           }
       }
       /// Get the value of the given [node prop](#common.NodeProp) for this
       /// node. Works with both per-node and per-type props.
       prop(prop) {
           return !prop.perNode ? this.type.prop(prop) : this.props ? this.props[prop.id] : undefined;
       }
       /// Returns the node's [per-node props](#common.NodeProp.perNode) in a
       /// format that can be passed to the [`Tree`](#common.Tree)
       /// constructor.
       get propValues() {
           let result = [];
           if (this.props)
               for (let id in this.props)
                   result.push([+id, this.props[id]]);
           return result;
       }
       /// Balance the direct children of this tree, producing a copy of
       /// which may have children grouped into subtrees with type
       /// [`NodeType.none`](#common.NodeType^none).
       balance(config = {}) {
           return this.children.length <= 8 /* BranchFactor */ ? this :
               balanceRange(NodeType.none, this.children, this.positions, 0, this.children.length, 0, this.length, (children, positions, length) => new Tree(this.type, children, positions, length, this.propValues), config.makeTree || ((children, positions, length) => new Tree(NodeType.none, children, positions, length)));
       }
       /// Build a tree from a postfix-ordered buffer of node information,
       /// or a cursor over such a buffer.
       static build(data) { return buildTree(data); }
   }
   /// The empty tree
   Tree.empty = new Tree(NodeType.none, [], [], 0);
   class FlatBufferCursor {
       constructor(buffer, index) {
           this.buffer = buffer;
           this.index = index;
       }
       get id() { return this.buffer[this.index - 4]; }
       get start() { return this.buffer[this.index - 3]; }
       get end() { return this.buffer[this.index - 2]; }
       get size() { return this.buffer[this.index - 1]; }
       get pos() { return this.index; }
       next() { this.index -= 4; }
       fork() { return new FlatBufferCursor(this.buffer, this.index); }
   }
   /// Tree buffers contain (type, start, end, endIndex) quads for each
   /// node. In such a buffer, nodes are stored in prefix order (parents
   /// before children, with the endIndex of the parent indicating which
   /// children belong to it).
   class TreeBuffer {
       /// Create a tree buffer.
       constructor(
       /// The buffer's content.
       buffer, 
       /// The total length of the group of nodes in the buffer.
       length, 
       /// The node set used in this buffer.
       set) {
           this.buffer = buffer;
           this.length = length;
           this.set = set;
       }
       /// @internal
       get type() { return NodeType.none; }
       /// @internal
       toString() {
           let result = [];
           for (let index = 0; index < this.buffer.length;) {
               result.push(this.childString(index));
               index = this.buffer[index + 3];
           }
           return result.join(",");
       }
       /// @internal
       childString(index) {
           let id = this.buffer[index], endIndex = this.buffer[index + 3];
           let type = this.set.types[id], result = type.name;
           if (/\W/.test(result) && !type.isError)
               result = JSON.stringify(result);
           index += 4;
           if (endIndex == index)
               return result;
           let children = [];
           while (index < endIndex) {
               children.push(this.childString(index));
               index = this.buffer[index + 3];
           }
           return result + "(" + children.join(",") + ")";
       }
       /// @internal
       findChild(startIndex, endIndex, dir, pos, side) {
           let { buffer } = this, pick = -1;
           for (let i = startIndex; i != endIndex; i = buffer[i + 3]) {
               if (checkSide(side, pos, buffer[i + 1], buffer[i + 2])) {
                   pick = i;
                   if (dir > 0)
                       break;
               }
           }
           return pick;
       }
       /// @internal
       slice(startI, endI, from, to) {
           let b = this.buffer;
           let copy = new Uint16Array(endI - startI);
           for (let i = startI, j = 0; i < endI;) {
               copy[j++] = b[i++];
               copy[j++] = b[i++] - from;
               copy[j++] = b[i++] - from;
               copy[j++] = b[i++] - startI;
           }
           return new TreeBuffer(copy, to - from, this.set);
       }
   }
   function checkSide(side, pos, from, to) {
       switch (side) {
           case -2 /* Before */: return from < pos;
           case -1 /* AtOrBefore */: return to >= pos && from < pos;
           case 0 /* Around */: return from < pos && to > pos;
           case 1 /* AtOrAfter */: return from <= pos && to > pos;
           case 2 /* After */: return to > pos;
           case 4 /* DontCare */: return true;
       }
   }
   function enterUnfinishedNodesBefore(node, pos) {
       let scan = node.childBefore(pos);
       while (scan) {
           let last = scan.lastChild;
           if (!last || last.to != scan.to)
               break;
           if (last.type.isError && last.from == last.to) {
               node = scan;
               scan = last.prevSibling;
           }
           else {
               scan = last;
           }
       }
       return node;
   }
   function resolveNode(node, pos, side, overlays) {
       var _a;
       // Move up to a node that actually holds the position, if possible
       while (node.from == node.to ||
           (side < 1 ? node.from >= pos : node.from > pos) ||
           (side > -1 ? node.to <= pos : node.to < pos)) {
           let parent = !overlays && node instanceof TreeNode && node.index < 0 ? null : node.parent;
           if (!parent)
               return node;
           node = parent;
       }
       let mode = overlays ? 0 : IterMode.IgnoreOverlays;
       // Must go up out of overlays when those do not overlap with pos
       if (overlays)
           for (let scan = node, parent = scan.parent; parent; scan = parent, parent = scan.parent) {
               if (scan instanceof TreeNode && scan.index < 0 && ((_a = parent.enter(pos, side, mode)) === null || _a === void 0 ? void 0 : _a.from) != scan.from)
                   node = parent;
           }
       for (;;) {
           let inner = node.enter(pos, side, mode);
           if (!inner)
               return node;
           node = inner;
       }
   }
   class TreeNode {
       constructor(_tree, from, 
       // Index in parent node, set to -1 if the node is not a direct child of _parent.node (overlay)
       index, _parent) {
           this._tree = _tree;
           this.from = from;
           this.index = index;
           this._parent = _parent;
       }
       get type() { return this._tree.type; }
       get name() { return this._tree.type.name; }
       get to() { return this.from + this._tree.length; }
       nextChild(i, dir, pos, side, mode = 0) {
           for (let parent = this;;) {
               for (let { children, positions } = parent._tree, e = dir > 0 ? children.length : -1; i != e; i += dir) {
                   let next = children[i], start = positions[i] + parent.from;
                   if (!checkSide(side, pos, start, start + next.length))
                       continue;
                   if (next instanceof TreeBuffer) {
                       if (mode & IterMode.ExcludeBuffers)
                           continue;
                       let index = next.findChild(0, next.buffer.length, dir, pos - start, side);
                       if (index > -1)
                           return new BufferNode(new BufferContext(parent, next, i, start), null, index);
                   }
                   else if ((mode & IterMode.IncludeAnonymous) || (!next.type.isAnonymous || hasChild(next))) {
                       let mounted;
                       if (!(mode & IterMode.IgnoreMounts) &&
                           next.props && (mounted = next.prop(NodeProp.mounted)) && !mounted.overlay)
                           return new TreeNode(mounted.tree, start, i, parent);
                       let inner = new TreeNode(next, start, i, parent);
                       return (mode & IterMode.IncludeAnonymous) || !inner.type.isAnonymous ? inner
                           : inner.nextChild(dir < 0 ? next.children.length - 1 : 0, dir, pos, side);
                   }
               }
               if ((mode & IterMode.IncludeAnonymous) || !parent.type.isAnonymous)
                   return null;
               if (parent.index >= 0)
                   i = parent.index + dir;
               else
                   i = dir < 0 ? -1 : parent._parent._tree.children.length;
               parent = parent._parent;
               if (!parent)
                   return null;
           }
       }
       get firstChild() { return this.nextChild(0, 1, 0, 4 /* DontCare */); }
       get lastChild() { return this.nextChild(this._tree.children.length - 1, -1, 0, 4 /* DontCare */); }
       childAfter(pos) { return this.nextChild(0, 1, pos, 2 /* After */); }
       childBefore(pos) { return this.nextChild(this._tree.children.length - 1, -1, pos, -2 /* Before */); }
       enter(pos, side, mode = 0) {
           let mounted;
           if (!(mode & IterMode.IgnoreOverlays) && (mounted = this._tree.prop(NodeProp.mounted)) && mounted.overlay) {
               let rPos = pos - this.from;
               for (let { from, to } of mounted.overlay) {
                   if ((side > 0 ? from <= rPos : from < rPos) &&
                       (side < 0 ? to >= rPos : to > rPos))
                       return new TreeNode(mounted.tree, mounted.overlay[0].from + this.from, -1, this);
               }
           }
           return this.nextChild(0, 1, pos, side, mode);
       }
       nextSignificantParent() {
           let val = this;
           while (val.type.isAnonymous && val._parent)
               val = val._parent;
           return val;
       }
       get parent() {
           return this._parent ? this._parent.nextSignificantParent() : null;
       }
       get nextSibling() {
           return this._parent && this.index >= 0 ? this._parent.nextChild(this.index + 1, 1, 0, 4 /* DontCare */) : null;
       }
       get prevSibling() {
           return this._parent && this.index >= 0 ? this._parent.nextChild(this.index - 1, -1, 0, 4 /* DontCare */) : null;
       }
       cursor(mode = 0) { return new TreeCursor(this, mode); }
       get tree() { return this._tree; }
       toTree() { return this._tree; }
       resolve(pos, side = 0) {
           return resolveNode(this, pos, side, false);
       }
       resolveInner(pos, side = 0) {
           return resolveNode(this, pos, side, true);
       }
       enterUnfinishedNodesBefore(pos) { return enterUnfinishedNodesBefore(this, pos); }
       getChild(type, before = null, after = null) {
           let r = getChildren(this, type, before, after);
           return r.length ? r[0] : null;
       }
       getChildren(type, before = null, after = null) {
           return getChildren(this, type, before, after);
       }
       /// @internal
       toString() { return this._tree.toString(); }
       get node() { return this; }
       matchContext(context) { return matchNodeContext(this, context); }
   }
   function getChildren(node, type, before, after) {
       let cur = node.cursor(), result = [];
       if (!cur.firstChild())
           return result;
       if (before != null)
           while (!cur.type.is(before))
               if (!cur.nextSibling())
                   return result;
       for (;;) {
           if (after != null && cur.type.is(after))
               return result;
           if (cur.type.is(type))
               result.push(cur.node);
           if (!cur.nextSibling())
               return after == null ? result : [];
       }
   }
   function matchNodeContext(node, context, i = context.length - 1) {
       for (let p = node.parent; i >= 0; p = p.parent) {
           if (!p)
               return false;
           if (!p.type.isAnonymous) {
               if (context[i] && context[i] != p.name)
                   return false;
               i--;
           }
       }
       return true;
   }
   class BufferContext {
       constructor(parent, buffer, index, start) {
           this.parent = parent;
           this.buffer = buffer;
           this.index = index;
           this.start = start;
       }
   }
   class BufferNode {
       constructor(context, _parent, index) {
           this.context = context;
           this._parent = _parent;
           this.index = index;
           this.type = context.buffer.set.types[context.buffer.buffer[index]];
       }
       get name() { return this.type.name; }
       get from() { return this.context.start + this.context.buffer.buffer[this.index + 1]; }
       get to() { return this.context.start + this.context.buffer.buffer[this.index + 2]; }
       child(dir, pos, side) {
           let { buffer } = this.context;
           let index = buffer.findChild(this.index + 4, buffer.buffer[this.index + 3], dir, pos - this.context.start, side);
           return index < 0 ? null : new BufferNode(this.context, this, index);
       }
       get firstChild() { return this.child(1, 0, 4 /* DontCare */); }
       get lastChild() { return this.child(-1, 0, 4 /* DontCare */); }
       childAfter(pos) { return this.child(1, pos, 2 /* After */); }
       childBefore(pos) { return this.child(-1, pos, -2 /* Before */); }
       enter(pos, side, mode = 0) {
           if (mode & IterMode.ExcludeBuffers)
               return null;
           let { buffer } = this.context;
           let index = buffer.findChild(this.index + 4, buffer.buffer[this.index + 3], side > 0 ? 1 : -1, pos - this.context.start, side);
           return index < 0 ? null : new BufferNode(this.context, this, index);
       }
       get parent() {
           return this._parent || this.context.parent.nextSignificantParent();
       }
       externalSibling(dir) {
           return this._parent ? null : this.context.parent.nextChild(this.context.index + dir, dir, 0, 4 /* DontCare */);
       }
       get nextSibling() {
           let { buffer } = this.context;
           let after = buffer.buffer[this.index + 3];
           if (after < (this._parent ? buffer.buffer[this._parent.index + 3] : buffer.buffer.length))
               return new BufferNode(this.context, this._parent, after);
           return this.externalSibling(1);
       }
       get prevSibling() {
           let { buffer } = this.context;
           let parentStart = this._parent ? this._parent.index + 4 : 0;
           if (this.index == parentStart)
               return this.externalSibling(-1);
           return new BufferNode(this.context, this._parent, buffer.findChild(parentStart, this.index, -1, 0, 4 /* DontCare */));
       }
       cursor(mode = 0) { return new TreeCursor(this, mode); }
       get tree() { return null; }
       toTree() {
           let children = [], positions = [];
           let { buffer } = this.context;
           let startI = this.index + 4, endI = buffer.buffer[this.index + 3];
           if (endI > startI) {
               let from = buffer.buffer[this.index + 1], to = buffer.buffer[this.index + 2];
               children.push(buffer.slice(startI, endI, from, to));
               positions.push(0);
           }
           return new Tree(this.type, children, positions, this.to - this.from);
       }
       resolve(pos, side = 0) {
           return resolveNode(this, pos, side, false);
       }
       resolveInner(pos, side = 0) {
           return resolveNode(this, pos, side, true);
       }
       enterUnfinishedNodesBefore(pos) { return enterUnfinishedNodesBefore(this, pos); }
       /// @internal
       toString() { return this.context.buffer.childString(this.index); }
       getChild(type, before = null, after = null) {
           let r = getChildren(this, type, before, after);
           return r.length ? r[0] : null;
       }
       getChildren(type, before = null, after = null) {
           return getChildren(this, type, before, after);
       }
       get node() { return this; }
       matchContext(context) { return matchNodeContext(this, context); }
   }
   /// A tree cursor object focuses on a given node in a syntax tree, and
   /// allows you to move to adjacent nodes.
   class TreeCursor {
       /// @internal
       constructor(node, 
       /// @internal
       mode = 0) {
           this.mode = mode;
           /// @internal
           this.buffer = null;
           this.stack = [];
           /// @internal
           this.index = 0;
           this.bufferNode = null;
           if (node instanceof TreeNode) {
               this.yieldNode(node);
           }
           else {
               this._tree = node.context.parent;
               this.buffer = node.context;
               for (let n = node._parent; n; n = n._parent)
                   this.stack.unshift(n.index);
               this.bufferNode = node;
               this.yieldBuf(node.index);
           }
       }
       /// Shorthand for `.type.name`.
       get name() { return this.type.name; }
       yieldNode(node) {
           if (!node)
               return false;
           this._tree = node;
           this.type = node.type;
           this.from = node.from;
           this.to = node.to;
           return true;
       }
       yieldBuf(index, type) {
           this.index = index;
           let { start, buffer } = this.buffer;
           this.type = type || buffer.set.types[buffer.buffer[index]];
           this.from = start + buffer.buffer[index + 1];
           this.to = start + buffer.buffer[index + 2];
           return true;
       }
       yield(node) {
           if (!node)
               return false;
           if (node instanceof TreeNode) {
               this.buffer = null;
               return this.yieldNode(node);
           }
           this.buffer = node.context;
           return this.yieldBuf(node.index, node.type);
       }
       /// @internal
       toString() {
           return this.buffer ? this.buffer.buffer.childString(this.index) : this._tree.toString();
       }
       /// @internal
       enterChild(dir, pos, side) {
           if (!this.buffer)
               return this.yield(this._tree.nextChild(dir < 0 ? this._tree._tree.children.length - 1 : 0, dir, pos, side, this.mode));
           let { buffer } = this.buffer;
           let index = buffer.findChild(this.index + 4, buffer.buffer[this.index + 3], dir, pos - this.buffer.start, side);
           if (index < 0)
               return false;
           this.stack.push(this.index);
           return this.yieldBuf(index);
       }
       /// Move the cursor to this node's first child. When this returns
       /// false, the node has no child, and the cursor has not been moved.
       firstChild() { return this.enterChild(1, 0, 4 /* DontCare */); }
       /// Move the cursor to this node's last child.
       lastChild() { return this.enterChild(-1, 0, 4 /* DontCare */); }
       /// Move the cursor to the first child that ends after `pos`.
       childAfter(pos) { return this.enterChild(1, pos, 2 /* After */); }
       /// Move to the last child that starts before `pos`.
       childBefore(pos) { return this.enterChild(-1, pos, -2 /* Before */); }
       /// Move the cursor to the child around `pos`. If side is -1 the
       /// child may end at that position, when 1 it may start there. This
       /// will also enter [overlaid](#common.MountedTree.overlay)
       /// [mounted](#common.NodeProp^mounted) trees unless `overlays` is
       /// set to false.
       enter(pos, side, mode = this.mode) {
           if (!this.buffer)
               return this.yield(this._tree.enter(pos, side, mode));
           return mode & IterMode.ExcludeBuffers ? false : this.enterChild(1, pos, side);
       }
       /// Move to the node's parent node, if this isn't the top node.
       parent() {
           if (!this.buffer)
               return this.yieldNode((this.mode & IterMode.IncludeAnonymous) ? this._tree._parent : this._tree.parent);
           if (this.stack.length)
               return this.yieldBuf(this.stack.pop());
           let parent = (this.mode & IterMode.IncludeAnonymous) ? this.buffer.parent : this.buffer.parent.nextSignificantParent();
           this.buffer = null;
           return this.yieldNode(parent);
       }
       /// @internal
       sibling(dir) {
           if (!this.buffer)
               return !this._tree._parent ? false
                   : this.yield(this._tree.index < 0 ? null
                       : this._tree._parent.nextChild(this._tree.index + dir, dir, 0, 4 /* DontCare */, this.mode));
           let { buffer } = this.buffer, d = this.stack.length - 1;
           if (dir < 0) {
               let parentStart = d < 0 ? 0 : this.stack[d] + 4;
               if (this.index != parentStart)
                   return this.yieldBuf(buffer.findChild(parentStart, this.index, -1, 0, 4 /* DontCare */));
           }
           else {
               let after = buffer.buffer[this.index + 3];
               if (after < (d < 0 ? buffer.buffer.length : buffer.buffer[this.stack[d] + 3]))
                   return this.yieldBuf(after);
           }
           return d < 0 ? this.yield(this.buffer.parent.nextChild(this.buffer.index + dir, dir, 0, 4 /* DontCare */, this.mode)) : false;
       }
       /// Move to this node's next sibling, if any.
       nextSibling() { return this.sibling(1); }
       /// Move to this node's previous sibling, if any.
       prevSibling() { return this.sibling(-1); }
       atLastNode(dir) {
           let index, parent, { buffer } = this;
           if (buffer) {
               if (dir > 0) {
                   if (this.index < buffer.buffer.buffer.length)
                       return false;
               }
               else {
                   for (let i = 0; i < this.index; i++)
                       if (buffer.buffer.buffer[i + 3] < this.index)
                           return false;
               }
               ({ index, parent } = buffer);
           }
           else {
               ({ index, _parent: parent } = this._tree);
           }
           for (; parent; { index, _parent: parent } = parent) {
               if (index > -1)
                   for (let i = index + dir, e = dir < 0 ? -1 : parent._tree.children.length; i != e; i += dir) {
                       let child = parent._tree.children[i];
                       if ((this.mode & IterMode.IncludeAnonymous) ||
                           child instanceof TreeBuffer ||
                           !child.type.isAnonymous ||
                           hasChild(child))
                           return false;
                   }
           }
           return true;
       }
       move(dir, enter) {
           if (enter && this.enterChild(dir, 0, 4 /* DontCare */))
               return true;
           for (;;) {
               if (this.sibling(dir))
                   return true;
               if (this.atLastNode(dir) || !this.parent())
                   return false;
           }
       }
       /// Move to the next node in a
       /// [pre-order](https://en.wikipedia.org/wiki/Tree_traversal#Pre-order_(NLR))
       /// traversal, going from a node to its first child or, if the
       /// current node is empty or `enter` is false, its next sibling or
       /// the next sibling of the first parent node that has one.
       next(enter = true) { return this.move(1, enter); }
       /// Move to the next node in a last-to-first pre-order traveral. A
       /// node is followed by its last child or, if it has none, its
       /// previous sibling or the previous sibling of the first parent
       /// node that has one.
       prev(enter = true) { return this.move(-1, enter); }
       /// Move the cursor to the innermost node that covers `pos`. If
       /// `side` is -1, it will enter nodes that end at `pos`. If it is 1,
       /// it will enter nodes that start at `pos`.
       moveTo(pos, side = 0) {
           // Move up to a node that actually holds the position, if possible
           while (this.from == this.to ||
               (side < 1 ? this.from >= pos : this.from > pos) ||
               (side > -1 ? this.to <= pos : this.to < pos))
               if (!this.parent())
                   break;
           // Then scan down into child nodes as far as possible
           while (this.enterChild(1, pos, side)) { }
           return this;
       }
       /// Get a [syntax node](#common.SyntaxNode) at the cursor's current
       /// position.
       get node() {
           if (!this.buffer)
               return this._tree;
           let cache = this.bufferNode, result = null, depth = 0;
           if (cache && cache.context == this.buffer) {
               scan: for (let index = this.index, d = this.stack.length; d >= 0;) {
                   for (let c = cache; c; c = c._parent)
                       if (c.index == index) {
                           if (index == this.index)
                               return c;
                           result = c;
                           depth = d + 1;
                           break scan;
                       }
                   index = this.stack[--d];
               }
           }
           for (let i = depth; i < this.stack.length; i++)
               result = new BufferNode(this.buffer, result, this.stack[i]);
           return this.bufferNode = new BufferNode(this.buffer, result, this.index);
       }
       /// Get the [tree](#common.Tree) that represents the current node, if
       /// any. Will return null when the node is in a [tree
       /// buffer](#common.TreeBuffer).
       get tree() {
           return this.buffer ? null : this._tree._tree;
       }
       /// Iterate over the current node and all its descendants, calling
       /// `enter` when entering a node and `leave`, if given, when leaving
       /// one. When `enter` returns `false`, any children of that node are
       /// skipped, and `leave` isn't called for it.
       iterate(enter, leave) {
           for (let depth = 0;;) {
               let mustLeave = false;
               if (this.type.isAnonymous || enter(this) !== false) {
                   if (this.firstChild()) {
                       depth++;
                       continue;
                   }
                   if (!this.type.isAnonymous)
                       mustLeave = true;
               }
               for (;;) {
                   if (mustLeave && leave)
                       leave(this);
                   mustLeave = this.type.isAnonymous;
                   if (this.nextSibling())
                       break;
                   if (!depth)
                       return;
                   this.parent();
                   depth--;
                   mustLeave = true;
               }
           }
       }
       /// Test whether the current node matches a given context—a sequence
       /// of direct parent node names. Empty strings in the context array
       /// are treated as wildcards.
       matchContext(context) {
           if (!this.buffer)
               return matchNodeContext(this.node, context);
           let { buffer } = this.buffer, { types } = buffer.set;
           for (let i = context.length - 1, d = this.stack.length - 1; i >= 0; d--) {
               if (d < 0)
                   return matchNodeContext(this.node, context, i);
               let type = types[buffer.buffer[this.stack[d]]];
               if (!type.isAnonymous) {
                   if (context[i] && context[i] != type.name)
                       return false;
                   i--;
               }
           }
           return true;
       }
   }
   function hasChild(tree) {
       return tree.children.some(ch => ch instanceof TreeBuffer || !ch.type.isAnonymous || hasChild(ch));
   }
   function buildTree(data) {
       var _a;
       let { buffer, nodeSet, maxBufferLength = DefaultBufferLength, reused = [], minRepeatType = nodeSet.types.length } = data;
       let cursor = Array.isArray(buffer) ? new FlatBufferCursor(buffer, buffer.length) : buffer;
       let types = nodeSet.types;
       let contextHash = 0, lookAhead = 0;
       function takeNode(parentStart, minPos, children, positions, inRepeat) {
           let { id, start, end, size } = cursor;
           let lookAheadAtStart = lookAhead;
           while (size < 0) {
               cursor.next();
               if (size == -1 /* Reuse */) {
                   let node = reused[id];
                   children.push(node);
                   positions.push(start - parentStart);
                   return;
               }
               else if (size == -3 /* ContextChange */) { // Context change
                   contextHash = id;
                   return;
               }
               else if (size == -4 /* LookAhead */) {
                   lookAhead = id;
                   return;
               }
               else {
                   throw new RangeError(`Unrecognized record size: ${size}`);
               }
           }
           let type = types[id], node, buffer;
           let startPos = start - parentStart;
           if (end - start <= maxBufferLength && (buffer = findBufferSize(cursor.pos - minPos, inRepeat))) {
               // Small enough for a buffer, and no reused nodes inside
               let data = new Uint16Array(buffer.size - buffer.skip);
               let endPos = cursor.pos - buffer.size, index = data.length;
               while (cursor.pos > endPos)
                   index = copyToBuffer(buffer.start, data, index);
               node = new TreeBuffer(data, end - buffer.start, nodeSet);
               startPos = buffer.start - parentStart;
           }
           else { // Make it a node
               let endPos = cursor.pos - size;
               cursor.next();
               let localChildren = [], localPositions = [];
               let localInRepeat = id >= minRepeatType ? id : -1;
               let lastGroup = 0, lastEnd = end;
               while (cursor.pos > endPos) {
                   if (localInRepeat >= 0 && cursor.id == localInRepeat && cursor.size >= 0) {
                       if (cursor.end <= lastEnd - maxBufferLength) {
                           makeRepeatLeaf(localChildren, localPositions, start, lastGroup, cursor.end, lastEnd, localInRepeat, lookAheadAtStart);
                           lastGroup = localChildren.length;
                           lastEnd = cursor.end;
                       }
                       cursor.next();
                   }
                   else {
                       takeNode(start, endPos, localChildren, localPositions, localInRepeat);
                   }
               }
               if (localInRepeat >= 0 && lastGroup > 0 && lastGroup < localChildren.length)
                   makeRepeatLeaf(localChildren, localPositions, start, lastGroup, start, lastEnd, localInRepeat, lookAheadAtStart);
               localChildren.reverse();
               localPositions.reverse();
               if (localInRepeat > -1 && lastGroup > 0) {
                   let make = makeBalanced(type);
                   node = balanceRange(type, localChildren, localPositions, 0, localChildren.length, 0, end - start, make, make);
               }
               else {
                   node = makeTree(type, localChildren, localPositions, end - start, lookAheadAtStart - end);
               }
           }
           children.push(node);
           positions.push(startPos);
       }
       function makeBalanced(type) {
           return (children, positions, length) => {
               let lookAhead = 0, lastI = children.length - 1, last, lookAheadProp;
               if (lastI >= 0 && (last = children[lastI]) instanceof Tree) {
                   if (!lastI && last.type == type && last.length == length)
                       return last;
                   if (lookAheadProp = last.prop(NodeProp.lookAhead))
                       lookAhead = positions[lastI] + last.length + lookAheadProp;
               }
               return makeTree(type, children, positions, length, lookAhead);
           };
       }
       function makeRepeatLeaf(children, positions, base, i, from, to, type, lookAhead) {
           let localChildren = [], localPositions = [];
           while (children.length > i) {
               localChildren.push(children.pop());
               localPositions.push(positions.pop() + base - from);
           }
           children.push(makeTree(nodeSet.types[type], localChildren, localPositions, to - from, lookAhead - to));
           positions.push(from - base);
       }
       function makeTree(type, children, positions, length, lookAhead = 0, props) {
           if (contextHash) {
               let pair = [NodeProp.contextHash, contextHash];
               props = props ? [pair].concat(props) : [pair];
           }
           if (lookAhead > 25) {
               let pair = [NodeProp.lookAhead, lookAhead];
               props = props ? [pair].concat(props) : [pair];
           }
           return new Tree(type, children, positions, length, props);
       }
       function findBufferSize(maxSize, inRepeat) {
           // Scan through the buffer to find previous siblings that fit
           // together in a TreeBuffer, and don't contain any reused nodes
           // (which can't be stored in a buffer).
           // If `inRepeat` is > -1, ignore node boundaries of that type for
           // nesting, but make sure the end falls either at the start
           // (`maxSize`) or before such a node.
           let fork = cursor.fork();
           let size = 0, start = 0, skip = 0, minStart = fork.end - maxBufferLength;
           let result = { size: 0, start: 0, skip: 0 };
           scan: for (let minPos = fork.pos - maxSize; fork.pos > minPos;) {
               let nodeSize = fork.size;
               // Pretend nested repeat nodes of the same type don't exist
               if (fork.id == inRepeat && nodeSize >= 0) {
                   // Except that we store the current state as a valid return
                   // value.
                   result.size = size;
                   result.start = start;
                   result.skip = skip;
                   skip += 4;
                   size += 4;
                   fork.next();
                   continue;
               }
               let startPos = fork.pos - nodeSize;
               if (nodeSize < 0 || startPos < minPos || fork.start < minStart)
                   break;
               let localSkipped = fork.id >= minRepeatType ? 4 : 0;
               let nodeStart = fork.start;
               fork.next();
               while (fork.pos > startPos) {
                   if (fork.size < 0) {
                       if (fork.size == -3 /* ContextChange */)
                           localSkipped += 4;
                       else
                           break scan;
                   }
                   else if (fork.id >= minRepeatType) {
                       localSkipped += 4;
                   }
                   fork.next();
               }
               start = nodeStart;
               size += nodeSize;
               skip += localSkipped;
           }
           if (inRepeat < 0 || size == maxSize) {
               result.size = size;
               result.start = start;
               result.skip = skip;
           }
           return result.size > 4 ? result : undefined;
       }
       function copyToBuffer(bufferStart, buffer, index) {
           let { id, start, end, size } = cursor;
           cursor.next();
           if (size >= 0 && id < minRepeatType) {
               let startIndex = index;
               if (size > 4) {
                   let endPos = cursor.pos - (size - 4);
                   while (cursor.pos > endPos)
                       index = copyToBuffer(bufferStart, buffer, index);
               }
               buffer[--index] = startIndex;
               buffer[--index] = end - bufferStart;
               buffer[--index] = start - bufferStart;
               buffer[--index] = id;
           }
           else if (size == -3 /* ContextChange */) {
               contextHash = id;
           }
           else if (size == -4 /* LookAhead */) {
               lookAhead = id;
           }
           return index;
       }
       let children = [], positions = [];
       while (cursor.pos > 0)
           takeNode(data.start || 0, data.bufferStart || 0, children, positions, -1);
       let length = (_a = data.length) !== null && _a !== void 0 ? _a : (children.length ? positions[0] + children[0].length : 0);
       return new Tree(types[data.topID], children.reverse(), positions.reverse(), length);
   }
   const nodeSizeCache = new WeakMap;
   function nodeSize(balanceType, node) {
       if (!balanceType.isAnonymous || node instanceof TreeBuffer || node.type != balanceType)
           return 1;
       let size = nodeSizeCache.get(node);
       if (size == null) {
           size = 1;
           for (let child of node.children) {
               if (child.type != balanceType || !(child instanceof Tree)) {
                   size = 1;
                   break;
               }
               size += nodeSize(balanceType, child);
           }
           nodeSizeCache.set(node, size);
       }
       return size;
   }
   function balanceRange(
   // The type the balanced tree's inner nodes.
   balanceType, 
   // The direct children and their positions
   children, positions, 
   // The index range in children/positions to use
   from, to, 
   // The start position of the nodes, relative to their parent.
   start, 
   // Length of the outer node
   length, 
   // Function to build the top node of the balanced tree
   mkTop, 
   // Function to build internal nodes for the balanced tree
   mkTree) {
       let total = 0;
       for (let i = from; i < to; i++)
           total += nodeSize(balanceType, children[i]);
       let maxChild = Math.ceil((total * 1.5) / 8 /* BranchFactor */);
       let localChildren = [], localPositions = [];
       function divide(children, positions, from, to, offset) {
           for (let i = from; i < to;) {
               let groupFrom = i, groupStart = positions[i], groupSize = nodeSize(balanceType, children[i]);
               i++;
               for (; i < to; i++) {
                   let nextSize = nodeSize(balanceType, children[i]);
                   if (groupSize + nextSize >= maxChild)
                       break;
                   groupSize += nextSize;
               }
               if (i == groupFrom + 1) {
                   if (groupSize > maxChild) {
                       let only = children[groupFrom]; // Only trees can have a size > 1
                       divide(only.children, only.positions, 0, only.children.length, positions[groupFrom] + offset);
                       continue;
                   }
                   localChildren.push(children[groupFrom]);
               }
               else {
                   let length = positions[i - 1] + children[i - 1].length - groupStart;
                   localChildren.push(balanceRange(balanceType, children, positions, groupFrom, i, groupStart, length, null, mkTree));
               }
               localPositions.push(groupStart + offset - start);
           }
       }
       divide(children, positions, from, to, 0);
       return (mkTop || mkTree)(localChildren, localPositions, length);
   }

   /// Tree fragments are used during [incremental
   /// parsing](#common.Parser.startParse) to track parts of old trees
   /// that can be reused in a new parse. An array of fragments is used
   /// to track regions of an old tree whose nodes might be reused in new
   /// parses. Use the static
   /// [`applyChanges`](#common.TreeFragment^applyChanges) method to
   /// update fragments for document changes.
   class TreeFragment {
       /// Construct a tree fragment. You'll usually want to use
       /// [`addTree`](#common.TreeFragment^addTree) and
       /// [`applyChanges`](#common.TreeFragment^applyChanges) instead of
       /// calling this directly.
       constructor(
       /// The start of the unchanged range pointed to by this fragment.
       /// This refers to an offset in the _updated_ document (as opposed
       /// to the original tree).
       from, 
       /// The end of the unchanged range.
       to, 
       /// The tree that this fragment is based on.
       tree, 
       /// The offset between the fragment's tree and the document that
       /// this fragment can be used against. Add this when going from
       /// document to tree positions, subtract it to go from tree to
       /// document positions.
       offset, openStart = false, openEnd = false) {
           this.from = from;
           this.to = to;
           this.tree = tree;
           this.offset = offset;
           this.open = (openStart ? 1 /* Start */ : 0) | (openEnd ? 2 /* End */ : 0);
       }
       /// Whether the start of the fragment represents the start of a
       /// parse, or the end of a change. (In the second case, it may not
       /// be safe to reuse some nodes at the start, depending on the
       /// parsing algorithm.)
       get openStart() { return (this.open & 1 /* Start */) > 0; }
       /// Whether the end of the fragment represents the end of a
       /// full-document parse, or the start of a change.
       get openEnd() { return (this.open & 2 /* End */) > 0; }
       /// Create a set of fragments from a freshly parsed tree, or update
       /// an existing set of fragments by replacing the ones that overlap
       /// with a tree with content from the new tree. When `partial` is
       /// true, the parse is treated as incomplete, and the resulting
       /// fragment has [`openEnd`](#common.TreeFragment.openEnd) set to
       /// true.
       static addTree(tree, fragments = [], partial = false) {
           let result = [new TreeFragment(0, tree.length, tree, 0, false, partial)];
           for (let f of fragments)
               if (f.to > tree.length)
                   result.push(f);
           return result;
       }
       /// Apply a set of edits to an array of fragments, removing or
       /// splitting fragments as necessary to remove edited ranges, and
       /// adjusting offsets for fragments that moved.
       static applyChanges(fragments, changes, minGap = 128) {
           if (!changes.length)
               return fragments;
           let result = [];
           let fI = 1, nextF = fragments.length ? fragments[0] : null;
           for (let cI = 0, pos = 0, off = 0;; cI++) {
               let nextC = cI < changes.length ? changes[cI] : null;
               let nextPos = nextC ? nextC.fromA : 1e9;
               if (nextPos - pos >= minGap)
                   while (nextF && nextF.from < nextPos) {
                       let cut = nextF;
                       if (pos >= cut.from || nextPos <= cut.to || off) {
                           let fFrom = Math.max(cut.from, pos) - off, fTo = Math.min(cut.to, nextPos) - off;
                           cut = fFrom >= fTo ? null : new TreeFragment(fFrom, fTo, cut.tree, cut.offset + off, cI > 0, !!nextC);
                       }
                       if (cut)
                           result.push(cut);
                       if (nextF.to > nextPos)
                           break;
                       nextF = fI < fragments.length ? fragments[fI++] : null;
                   }
               if (!nextC)
                   break;
               pos = nextC.toA;
               off = nextC.toA - nextC.toB;
           }
           return result;
       }
   }
   /// A superclass that parsers should extend.
   class Parser {
       /// Start a parse, returning a [partial parse](#common.PartialParse)
       /// object. [`fragments`](#common.TreeFragment) can be passed in to
       /// make the parse incremental.
       ///
       /// By default, the entire input is parsed. You can pass `ranges`,
       /// which should be a sorted array of non-empty, non-overlapping
       /// ranges, to parse only those ranges. The tree returned in that
       /// case will start at `ranges[0].from`.
       startParse(input, fragments, ranges) {
           if (typeof input == "string")
               input = new StringInput(input);
           ranges = !ranges ? [new Range(0, input.length)] : ranges.length ? ranges.map(r => new Range(r.from, r.to)) : [new Range(0, 0)];
           return this.createParse(input, fragments || [], ranges);
       }
       /// Run a full parse, returning the resulting tree.
       parse(input, fragments, ranges) {
           let parse = this.startParse(input, fragments, ranges);
           for (;;) {
               let done = parse.advance();
               if (done)
                   return done;
           }
       }
   }
   class StringInput {
       constructor(string) {
           this.string = string;
       }
       get length() { return this.string.length; }
       chunk(from) { return this.string.slice(from); }
       get lineChunks() { return false; }
       read(from, to) { return this.string.slice(from, to); }
   }
   new NodeProp({ perNode: true });

   let nextTagID = 0;
   /// Highlighting tags are markers that denote a highlighting category.
   /// They are [associated](#highlight.styleTags) with parts of a syntax
   /// tree by a language mode, and then mapped to an actual CSS style by
   /// a [highlighter](#highlight.Highlighter).
   ///
   /// Because syntax tree node types and highlight styles have to be
   /// able to talk the same language, CodeMirror uses a mostly _closed_
   /// [vocabulary](#highlight.tags) of syntax tags (as opposed to
   /// traditional open string-based systems, which make it hard for
   /// highlighting themes to cover all the tokens produced by the
   /// various languages).
   ///
   /// It _is_ possible to [define](#highlight.Tag^define) your own
   /// highlighting tags for system-internal use (where you control both
   /// the language package and the highlighter), but such tags will not
   /// be picked up by regular highlighters (though you can derive them
   /// from standard tags to allow highlighters to fall back to those).
   class Tag {
       /// @internal
       constructor(
       /// The set of this tag and all its parent tags, starting with
       /// this one itself and sorted in order of decreasing specificity.
       set, 
       /// The base unmodified tag that this one is based on, if it's
       /// modified @internal
       base, 
       /// The modifiers applied to this.base @internal
       modified) {
           this.set = set;
           this.base = base;
           this.modified = modified;
           /// @internal
           this.id = nextTagID++;
       }
       /// Define a new tag. If `parent` is given, the tag is treated as a
       /// sub-tag of that parent, and
       /// [highlighters](#highlight.tagHighlighter) that don't mention
       /// this tag will try to fall back to the parent tag (or grandparent
       /// tag, etc).
       static define(parent) {
           if (parent === null || parent === void 0 ? void 0 : parent.base)
               throw new Error("Can not derive from a modified tag");
           let tag = new Tag([], null, []);
           tag.set.push(tag);
           if (parent)
               for (let t of parent.set)
                   tag.set.push(t);
           return tag;
       }
       /// Define a tag _modifier_, which is a function that, given a tag,
       /// will return a tag that is a subtag of the original. Applying the
       /// same modifier to a twice tag will return the same value (`m1(t1)
       /// == m1(t1)`) and applying multiple modifiers will, regardless or
       /// order, produce the same tag (`m1(m2(t1)) == m2(m1(t1))`).
       ///
       /// When multiple modifiers are applied to a given base tag, each
       /// smaller set of modifiers is registered as a parent, so that for
       /// example `m1(m2(m3(t1)))` is a subtype of `m1(m2(t1))`,
       /// `m1(m3(t1)`, and so on.
       static defineModifier() {
           let mod = new Modifier;
           return (tag) => {
               if (tag.modified.indexOf(mod) > -1)
                   return tag;
               return Modifier.get(tag.base || tag, tag.modified.concat(mod).sort((a, b) => a.id - b.id));
           };
       }
   }
   let nextModifierID = 0;
   class Modifier {
       constructor() {
           this.instances = [];
           this.id = nextModifierID++;
       }
       static get(base, mods) {
           if (!mods.length)
               return base;
           let exists = mods[0].instances.find(t => t.base == base && sameArray(mods, t.modified));
           if (exists)
               return exists;
           let set = [], tag = new Tag(set, base, mods);
           for (let m of mods)
               m.instances.push(tag);
           let configs = permute(mods);
           for (let parent of base.set)
               for (let config of configs)
                   set.push(Modifier.get(parent, config));
           return tag;
       }
   }
   function sameArray(a, b) {
       return a.length == b.length && a.every((x, i) => x == b[i]);
   }
   function permute(array) {
       let result = [array];
       for (let i = 0; i < array.length; i++) {
           for (let a of permute(array.slice(0, i).concat(array.slice(i + 1))))
               result.push(a);
       }
       return result;
   }
   /// This function is used to add a set of tags to a language syntax
   /// via [`NodeSet.extend`](#common.NodeSet.extend) or
   /// [`LRParser.configure`](#lr.LRParser.configure).
   ///
   /// The argument object maps node selectors to [highlighting
   /// tags](#highlight.Tag) or arrays of tags.
   ///
   /// Node selectors may hold one or more (space-separated) node paths.
   /// Such a path can be a [node name](#common.NodeType.name), or
   /// multiple node names (or `*` wildcards) separated by slash
   /// characters, as in `"Block/Declaration/VariableName"`. Such a path
   /// matches the final node but only if its direct parent nodes are the
   /// other nodes mentioned. A `*` in such a path matches any parent,
   /// but only a single level—wildcards that match multiple parents
   /// aren't supported, both for efficiency reasons and because Lezer
   /// trees make it rather hard to reason about what they would match.)
   ///
   /// A path can be ended with `/...` to indicate that the tag assigned
   /// to the node should also apply to all child nodes, even if they
   /// match their own style (by default, only the innermost style is
   /// used).
   ///
   /// When a path ends in `!`, as in `Attribute!`, no further matching
   /// happens for the node's child nodes, and the entire node gets the
   /// given style.
   ///
   /// In this notation, node names that contain `/`, `!`, `*`, or `...`
   /// must be quoted as JSON strings.
   ///
   /// For example:
   ///
   /// ```javascript
   /// parser.withProps(
   ///   styleTags({
   ///     // Style Number and BigNumber nodes
   ///     "Number BigNumber": tags.number,
   ///     // Style Escape nodes whose parent is String
   ///     "String/Escape": tags.escape,
   ///     // Style anything inside Attributes nodes
   ///     "Attributes!": tags.meta,
   ///     // Add a style to all content inside Italic nodes
   ///     "Italic/...": tags.emphasis,
   ///     // Style InvalidString nodes as both `string` and `invalid`
   ///     "InvalidString": [tags.string, tags.invalid],
   ///     // Style the node named "/" as punctuation
   ///     '"/"': tags.punctuation
   ///   })
   /// )
   /// ```
   function styleTags(spec) {
       let byName = Object.create(null);
       for (let prop in spec) {
           let tags = spec[prop];
           if (!Array.isArray(tags))
               tags = [tags];
           for (let part of prop.split(" "))
               if (part) {
                   let pieces = [], mode = 2 /* Normal */, rest = part;
                   for (let pos = 0;;) {
                       if (rest == "..." && pos > 0 && pos + 3 == part.length) {
                           mode = 1 /* Inherit */;
                           break;
                       }
                       let m = /^"(?:[^"\\]|\\.)*?"|[^\/!]+/.exec(rest);
                       if (!m)
                           throw new RangeError("Invalid path: " + part);
                       pieces.push(m[0] == "*" ? "" : m[0][0] == '"' ? JSON.parse(m[0]) : m[0]);
                       pos += m[0].length;
                       if (pos == part.length)
                           break;
                       let next = part[pos++];
                       if (pos == part.length && next == "!") {
                           mode = 0 /* Opaque */;
                           break;
                       }
                       if (next != "/")
                           throw new RangeError("Invalid path: " + part);
                       rest = part.slice(pos);
                   }
                   let last = pieces.length - 1, inner = pieces[last];
                   if (!inner)
                       throw new RangeError("Invalid path: " + part);
                   let rule = new Rule(tags, mode, last > 0 ? pieces.slice(0, last) : null);
                   byName[inner] = rule.sort(byName[inner]);
               }
       }
       return ruleNodeProp.add(byName);
   }
   const ruleNodeProp = new NodeProp();
   class Rule {
       constructor(tags, mode, context, next) {
           this.tags = tags;
           this.mode = mode;
           this.context = context;
           this.next = next;
       }
       sort(other) {
           if (!other || other.depth < this.depth) {
               this.next = other;
               return this;
           }
           other.next = this.sort(other.next);
           return other;
       }
       get depth() { return this.context ? this.context.length : 0; }
   }
   /// Define a [highlighter](#highlight.Highlighter) from an array of
   /// tag/class pairs. Classes associated with more specific tags will
   /// take precedence.
   function tagHighlighter(tags, options) {
       let map = Object.create(null);
       for (let style of tags) {
           if (!Array.isArray(style.tag))
               map[style.tag.id] = style.class;
           else
               for (let tag of style.tag)
                   map[tag.id] = style.class;
       }
       let { scope, all = null } = options || {};
       return {
           style: (tags) => {
               let cls = all;
               for (let tag of tags) {
                   for (let sub of tag.set) {
                       let tagClass = map[sub.id];
                       if (tagClass) {
                           cls = cls ? cls + " " + tagClass : tagClass;
                           break;
                       }
                   }
               }
               return cls;
           },
           scope: scope
       };
   }
   function highlightTags(highlighters, tags) {
       let result = null;
       for (let highlighter of highlighters) {
           let value = highlighter.style(tags);
           if (value)
               result = result ? result + " " + value : value;
       }
       return result;
   }
   /// Highlight the given [tree](#common.Tree) with the given
   /// [highlighter](#highlight.Highlighter).
   function highlightTree(tree, highlighter, 
   /// Assign styling to a region of the text. Will be called, in order
   /// of position, for any ranges where more than zero classes apply.
   /// `classes` is a space separated string of CSS classes.
   putStyle, 
   /// The start of the range to highlight.
   from = 0, 
   /// The end of the range.
   to = tree.length) {
       let builder = new HighlightBuilder(from, Array.isArray(highlighter) ? highlighter : [highlighter], putStyle);
       builder.highlightRange(tree.cursor(), from, to, "", builder.highlighters);
       builder.flush(to);
   }
   class HighlightBuilder {
       constructor(at, highlighters, span) {
           this.at = at;
           this.highlighters = highlighters;
           this.span = span;
           this.class = "";
       }
       startSpan(at, cls) {
           if (cls != this.class) {
               this.flush(at);
               if (at > this.at)
                   this.at = at;
               this.class = cls;
           }
       }
       flush(to) {
           if (to > this.at && this.class)
               this.span(this.at, to, this.class);
       }
       highlightRange(cursor, from, to, inheritedClass, highlighters) {
           let { type, from: start, to: end } = cursor;
           if (start >= to || end <= from)
               return;
           if (type.isTop)
               highlighters = this.highlighters.filter(h => !h.scope || h.scope(type));
           let cls = inheritedClass;
           let rule = type.prop(ruleNodeProp), opaque = false;
           while (rule) {
               if (!rule.context || cursor.matchContext(rule.context)) {
                   let tagCls = highlightTags(highlighters, rule.tags);
                   if (tagCls) {
                       if (cls)
                           cls += " ";
                       cls += tagCls;
                       if (rule.mode == 1 /* Inherit */)
                           inheritedClass += (inheritedClass ? " " : "") + tagCls;
                       else if (rule.mode == 0 /* Opaque */)
                           opaque = true;
                   }
                   break;
               }
               rule = rule.next;
           }
           this.startSpan(cursor.from, cls);
           if (opaque)
               return;
           let mounted = cursor.tree && cursor.tree.prop(NodeProp.mounted);
           if (mounted && mounted.overlay) {
               let inner = cursor.node.enter(mounted.overlay[0].from + start, 1);
               let innerHighlighters = this.highlighters.filter(h => !h.scope || h.scope(mounted.tree.type));
               let hasChild = cursor.firstChild();
               for (let i = 0, pos = start;; i++) {
                   let next = i < mounted.overlay.length ? mounted.overlay[i] : null;
                   let nextPos = next ? next.from + start : end;
                   let rangeFrom = Math.max(from, pos), rangeTo = Math.min(to, nextPos);
                   if (rangeFrom < rangeTo && hasChild) {
                       while (cursor.from < rangeTo) {
                           this.highlightRange(cursor, rangeFrom, rangeTo, inheritedClass, highlighters);
                           this.startSpan(Math.min(to, cursor.to), cls);
                           if (cursor.to >= nextPos || !cursor.nextSibling())
                               break;
                       }
                   }
                   if (!next || nextPos > to)
                       break;
                   pos = next.to + start;
                   if (pos > from) {
                       this.highlightRange(inner.cursor(), Math.max(from, next.from + start), Math.min(to, pos), inheritedClass, innerHighlighters);
                       this.startSpan(pos, cls);
                   }
               }
               if (hasChild)
                   cursor.parent();
           }
           else if (cursor.firstChild()) {
               do {
                   if (cursor.to <= from)
                       continue;
                   if (cursor.from >= to)
                       break;
                   this.highlightRange(cursor, from, to, inheritedClass, highlighters);
                   this.startSpan(Math.min(to, cursor.to), cls);
               } while (cursor.nextSibling());
               cursor.parent();
           }
       }
   }
   const t = Tag.define;
   const comment = t(), name = t(), typeName = t(name), propertyName = t(name), literal = t(), string = t(literal), number = t(literal), content = t(), heading = t(content), keyword = t(), operator = t(), punctuation = t(), bracket = t(punctuation), meta = t();
   /// The default set of highlighting [tags](#highlight.Tag).
   ///
   /// This collection is heavily biased towards programming languages,
   /// and necessarily incomplete. A full ontology of syntactic
   /// constructs would fill a stack of books, and be impractical to
   /// write themes for. So try to make do with this set. If all else
   /// fails, [open an
   /// issue](https://github.com/codemirror/codemirror.next) to propose a
   /// new tag, or [define](#highlight.Tag^define) a local custom tag for
   /// your use case.
   ///
   /// Note that it is not obligatory to always attach the most specific
   /// tag possible to an element—if your grammar can't easily
   /// distinguish a certain type of element (such as a local variable),
   /// it is okay to style it as its more general variant (a variable).
   /// 
   /// For tags that extend some parent tag, the documentation links to
   /// the parent.
   const tags = {
       /// A comment.
       comment,
       /// A line [comment](#highlight.tags.comment).
       lineComment: t(comment),
       /// A block [comment](#highlight.tags.comment).
       blockComment: t(comment),
       /// A documentation [comment](#highlight.tags.comment).
       docComment: t(comment),
       /// Any kind of identifier.
       name,
       /// The [name](#highlight.tags.name) of a variable.
       variableName: t(name),
       /// A type [name](#highlight.tags.name).
       typeName: typeName,
       /// A tag name (subtag of [`typeName`](#highlight.tags.typeName)).
       tagName: t(typeName),
       /// A property or field [name](#highlight.tags.name).
       propertyName: propertyName,
       /// An attribute name (subtag of [`propertyName`](#highlight.tags.propertyName)).
       attributeName: t(propertyName),
       /// The [name](#highlight.tags.name) of a class.
       className: t(name),
       /// A label [name](#highlight.tags.name).
       labelName: t(name),
       /// A namespace [name](#highlight.tags.name).
       namespace: t(name),
       /// The [name](#highlight.tags.name) of a macro.
       macroName: t(name),
       /// A literal value.
       literal,
       /// A string [literal](#highlight.tags.literal).
       string,
       /// A documentation [string](#highlight.tags.string).
       docString: t(string),
       /// A character literal (subtag of [string](#highlight.tags.string)).
       character: t(string),
       /// An attribute value (subtag of [string](#highlight.tags.string)).
       attributeValue: t(string),
       /// A number [literal](#highlight.tags.literal).
       number,
       /// An integer [number](#highlight.tags.number) literal.
       integer: t(number),
       /// A floating-point [number](#highlight.tags.number) literal.
       float: t(number),
       /// A boolean [literal](#highlight.tags.literal).
       bool: t(literal),
       /// Regular expression [literal](#highlight.tags.literal).
       regexp: t(literal),
       /// An escape [literal](#highlight.tags.literal), for example a
       /// backslash escape in a string.
       escape: t(literal),
       /// A color [literal](#highlight.tags.literal).
       color: t(literal),
       /// A URL [literal](#highlight.tags.literal).
       url: t(literal),
       /// A language keyword.
       keyword,
       /// The [keyword](#highlight.tags.keyword) for the self or this
       /// object.
       self: t(keyword),
       /// The [keyword](#highlight.tags.keyword) for null.
       null: t(keyword),
       /// A [keyword](#highlight.tags.keyword) denoting some atomic value.
       atom: t(keyword),
       /// A [keyword](#highlight.tags.keyword) that represents a unit.
       unit: t(keyword),
       /// A modifier [keyword](#highlight.tags.keyword).
       modifier: t(keyword),
       /// A [keyword](#highlight.tags.keyword) that acts as an operator.
       operatorKeyword: t(keyword),
       /// A control-flow related [keyword](#highlight.tags.keyword).
       controlKeyword: t(keyword),
       /// A [keyword](#highlight.tags.keyword) that defines something.
       definitionKeyword: t(keyword),
       /// A [keyword](#highlight.tags.keyword) related to defining or
       /// interfacing with modules.
       moduleKeyword: t(keyword),
       /// An operator.
       operator,
       /// An [operator](#highlight.tags.operator) that defines something.
       derefOperator: t(operator),
       /// Arithmetic-related [operator](#highlight.tags.operator).
       arithmeticOperator: t(operator),
       /// Logical [operator](#highlight.tags.operator).
       logicOperator: t(operator),
       /// Bit [operator](#highlight.tags.operator).
       bitwiseOperator: t(operator),
       /// Comparison [operator](#highlight.tags.operator).
       compareOperator: t(operator),
       /// [Operator](#highlight.tags.operator) that updates its operand.
       updateOperator: t(operator),
       /// [Operator](#highlight.tags.operator) that defines something.
       definitionOperator: t(operator),
       /// Type-related [operator](#highlight.tags.operator).
       typeOperator: t(operator),
       /// Control-flow [operator](#highlight.tags.operator).
       controlOperator: t(operator),
       /// Program or markup punctuation.
       punctuation,
       /// [Punctuation](#highlight.tags.punctuation) that separates
       /// things.
       separator: t(punctuation),
       /// Bracket-style [punctuation](#highlight.tags.punctuation).
       bracket,
       /// Angle [brackets](#highlight.tags.bracket) (usually `<` and `>`
       /// tokens).
       angleBracket: t(bracket),
       /// Square [brackets](#highlight.tags.bracket) (usually `[` and `]`
       /// tokens).
       squareBracket: t(bracket),
       /// Parentheses (usually `(` and `)` tokens). Subtag of
       /// [bracket](#highlight.tags.bracket).
       paren: t(bracket),
       /// Braces (usually `{` and `}` tokens). Subtag of
       /// [bracket](#highlight.tags.bracket).
       brace: t(bracket),
       /// Content, for example plain text in XML or markup documents.
       content,
       /// [Content](#highlight.tags.content) that represents a heading.
       heading,
       /// A level 1 [heading](#highlight.tags.heading).
       heading1: t(heading),
       /// A level 2 [heading](#highlight.tags.heading).
       heading2: t(heading),
       /// A level 3 [heading](#highlight.tags.heading).
       heading3: t(heading),
       /// A level 4 [heading](#highlight.tags.heading).
       heading4: t(heading),
       /// A level 5 [heading](#highlight.tags.heading).
       heading5: t(heading),
       /// A level 6 [heading](#highlight.tags.heading).
       heading6: t(heading),
       /// A prose separator (such as a horizontal rule).
       contentSeparator: t(content),
       /// [Content](#highlight.tags.content) that represents a list.
       list: t(content),
       /// [Content](#highlight.tags.content) that represents a quote.
       quote: t(content),
       /// [Content](#highlight.tags.content) that is emphasized.
       emphasis: t(content),
       /// [Content](#highlight.tags.content) that is styled strong.
       strong: t(content),
       /// [Content](#highlight.tags.content) that is part of a link.
       link: t(content),
       /// [Content](#highlight.tags.content) that is styled as code or
       /// monospace.
       monospace: t(content),
       /// [Content](#highlight.tags.content) that has a strike-through
       /// style.
       strikethrough: t(content),
       /// Inserted text in a change-tracking format.
       inserted: t(),
       /// Deleted text.
       deleted: t(),
       /// Changed text.
       changed: t(),
       /// An invalid or unsyntactic element.
       invalid: t(),
       /// Metadata or meta-instruction.
       meta,
       /// [Metadata](#highlight.tags.meta) that applies to the entire
       /// document.
       documentMeta: t(meta),
       /// [Metadata](#highlight.tags.meta) that annotates or adds
       /// attributes to a given syntactic element.
       annotation: t(meta),
       /// Processing instruction or preprocessor directive. Subtag of
       /// [meta](#highlight.tags.meta).
       processingInstruction: t(meta),
       /// [Modifier](#highlight.Tag^defineModifier) that indicates that a
       /// given element is being defined. Expected to be used with the
       /// various [name](#highlight.tags.name) tags.
       definition: Tag.defineModifier(),
       /// [Modifier](#highlight.Tag^defineModifier) that indicates that
       /// something is constant. Mostly expected to be used with
       /// [variable names](#highlight.tags.variableName).
       constant: Tag.defineModifier(),
       /// [Modifier](#highlight.Tag^defineModifier) used to indicate that
       /// a [variable](#highlight.tags.variableName) or [property
       /// name](#highlight.tags.propertyName) is being called or defined
       /// as a function.
       function: Tag.defineModifier(),
       /// [Modifier](#highlight.Tag^defineModifier) that can be applied to
       /// [names](#highlight.tags.name) to indicate that they belong to
       /// the language's standard environment.
       standard: Tag.defineModifier(),
       /// [Modifier](#highlight.Tag^defineModifier) that indicates a given
       /// [names](#highlight.tags.name) is local to some scope.
       local: Tag.defineModifier(),
       /// A generic variant [modifier](#highlight.Tag^defineModifier) that
       /// can be used to tag language-specific alternative variants of
       /// some common tag. It is recommended for themes to define special
       /// forms of at least the [string](#highlight.tags.string) and
       /// [variable name](#highlight.tags.variableName) tags, since those
       /// come up a lot.
       special: Tag.defineModifier()
   };
   /// This is a highlighter that adds stable, predictable classes to
   /// tokens, for styling with external CSS.
   ///
   /// The following tags are mapped to their name prefixed with `"tok-"`
   /// (for example `"tok-comment"`):
   ///
   /// * [`link`](#highlight.tags.link)
   /// * [`heading`](#highlight.tags.heading)
   /// * [`emphasis`](#highlight.tags.emphasis)
   /// * [`strong`](#highlight.tags.strong)
   /// * [`keyword`](#highlight.tags.keyword)
   /// * [`atom`](#highlight.tags.atom)
   /// * [`bool`](#highlight.tags.bool)
   /// * [`url`](#highlight.tags.url)
   /// * [`labelName`](#highlight.tags.labelName)
   /// * [`inserted`](#highlight.tags.inserted)
   /// * [`deleted`](#highlight.tags.deleted)
   /// * [`literal`](#highlight.tags.literal)
   /// * [`string`](#highlight.tags.string)
   /// * [`number`](#highlight.tags.number)
   /// * [`variableName`](#highlight.tags.variableName)
   /// * [`typeName`](#highlight.tags.typeName)
   /// * [`namespace`](#highlight.tags.namespace)
   /// * [`className`](#highlight.tags.className)
   /// * [`macroName`](#highlight.tags.macroName)
   /// * [`propertyName`](#highlight.tags.propertyName)
   /// * [`operator`](#highlight.tags.operator)
   /// * [`comment`](#highlight.tags.comment)
   /// * [`meta`](#highlight.tags.meta)
   /// * [`punctuation`](#highlight.tags.punctuation)
   /// * [`invalid`](#highlight.tags.invalid)
   ///
   /// In addition, these mappings are provided:
   ///
   /// * [`regexp`](#highlight.tags.regexp),
   ///   [`escape`](#highlight.tags.escape), and
   ///   [`special`](#highlight.tags.special)[`(string)`](#highlight.tags.string)
   ///   are mapped to `"tok-string2"`
   /// * [`special`](#highlight.tags.special)[`(variableName)`](#highlight.tags.variableName)
   ///   to `"tok-variableName2"`
   /// * [`local`](#highlight.tags.local)[`(variableName)`](#highlight.tags.variableName)
   ///   to `"tok-variableName tok-local"`
   /// * [`definition`](#highlight.tags.definition)[`(variableName)`](#highlight.tags.variableName)
   ///   to `"tok-variableName tok-definition"`
   /// * [`definition`](#highlight.tags.definition)[`(propertyName)`](#highlight.tags.propertyName)
   ///   to `"tok-propertyName tok-definition"`
   tagHighlighter([
       { tag: tags.link, class: "tok-link" },
       { tag: tags.heading, class: "tok-heading" },
       { tag: tags.emphasis, class: "tok-emphasis" },
       { tag: tags.strong, class: "tok-strong" },
       { tag: tags.keyword, class: "tok-keyword" },
       { tag: tags.atom, class: "tok-atom" },
       { tag: tags.bool, class: "tok-bool" },
       { tag: tags.url, class: "tok-url" },
       { tag: tags.labelName, class: "tok-labelName" },
       { tag: tags.inserted, class: "tok-inserted" },
       { tag: tags.deleted, class: "tok-deleted" },
       { tag: tags.literal, class: "tok-literal" },
       { tag: tags.string, class: "tok-string" },
       { tag: tags.number, class: "tok-number" },
       { tag: [tags.regexp, tags.escape, tags.special(tags.string)], class: "tok-string2" },
       { tag: tags.variableName, class: "tok-variableName" },
       { tag: tags.local(tags.variableName), class: "tok-variableName tok-local" },
       { tag: tags.definition(tags.variableName), class: "tok-variableName tok-definition" },
       { tag: tags.special(tags.variableName), class: "tok-variableName2" },
       { tag: tags.definition(tags.propertyName), class: "tok-propertyName tok-definition" },
       { tag: tags.typeName, class: "tok-typeName" },
       { tag: tags.namespace, class: "tok-namespace" },
       { tag: tags.className, class: "tok-className" },
       { tag: tags.macroName, class: "tok-macroName" },
       { tag: tags.propertyName, class: "tok-propertyName" },
       { tag: tags.operator, class: "tok-operator" },
       { tag: tags.comment, class: "tok-comment" },
       { tag: tags.meta, class: "tok-meta" },
       { tag: tags.invalid, class: "tok-invalid" },
       { tag: tags.punctuation, class: "tok-punctuation" }
   ]);

   var _a;
   /**
   Node prop stored in a parser's top syntax node to provide the
   facet that stores language-specific data for that language.
   */
   const languageDataProp = /*@__PURE__*/new NodeProp();
   /**
   Helper function to define a facet (to be added to the top syntax
   node(s) for a language via
   [`languageDataProp`](https://codemirror.net/6/docs/ref/#language.languageDataProp)), that will be
   used to associate language data with the language. You
   probably only need this when subclassing
   [`Language`](https://codemirror.net/6/docs/ref/#language.Language).
   */
   function defineLanguageFacet(baseData) {
       return Facet.define({
           combine: baseData ? values => values.concat(baseData) : undefined
       });
   }
   /**
   A language object manages parsing and per-language
   [metadata](https://codemirror.net/6/docs/ref/#state.EditorState.languageDataAt). Parse data is
   managed as a [Lezer](https://lezer.codemirror.net) tree. The class
   can be used directly, via the [`LRLanguage`](https://codemirror.net/6/docs/ref/#language.LRLanguage)
   subclass for [Lezer](https://lezer.codemirror.net/) LR parsers, or
   via the [`StreamLanguage`](https://codemirror.net/6/docs/ref/#language.StreamLanguage) subclass
   for stream parsers.
   */
   class Language {
       /**
       Construct a language object. If you need to invoke this
       directly, first define a data facet with
       [`defineLanguageFacet`](https://codemirror.net/6/docs/ref/#language.defineLanguageFacet), and then
       configure your parser to [attach](https://codemirror.net/6/docs/ref/#language.languageDataProp) it
       to the language's outer syntax node.
       */
       constructor(
       /**
       The [language data](https://codemirror.net/6/docs/ref/#state.EditorState.languageDataAt) facet
       used for this language.
       */
       data, parser, extraExtensions = []) {
           this.data = data;
           // Kludge to define EditorState.tree as a debugging helper,
           // without the EditorState package actually knowing about
           // languages and lezer trees.
           if (!EditorState.prototype.hasOwnProperty("tree"))
               Object.defineProperty(EditorState.prototype, "tree", { get() { return syntaxTree(this); } });
           this.parser = parser;
           this.extension = [
               language.of(this),
               EditorState.languageData.of((state, pos, side) => state.facet(languageDataFacetAt(state, pos, side)))
           ].concat(extraExtensions);
       }
       /**
       Query whether this language is active at the given position.
       */
       isActiveAt(state, pos, side = -1) {
           return languageDataFacetAt(state, pos, side) == this.data;
       }
       /**
       Find the document regions that were parsed using this language.
       The returned regions will _include_ any nested languages rooted
       in this language, when those exist.
       */
       findRegions(state) {
           let lang = state.facet(language);
           if ((lang === null || lang === void 0 ? void 0 : lang.data) == this.data)
               return [{ from: 0, to: state.doc.length }];
           if (!lang || !lang.allowsNesting)
               return [];
           let result = [];
           let explore = (tree, from) => {
               if (tree.prop(languageDataProp) == this.data) {
                   result.push({ from, to: from + tree.length });
                   return;
               }
               let mount = tree.prop(NodeProp.mounted);
               if (mount) {
                   if (mount.tree.prop(languageDataProp) == this.data) {
                       if (mount.overlay)
                           for (let r of mount.overlay)
                               result.push({ from: r.from + from, to: r.to + from });
                       else
                           result.push({ from: from, to: from + tree.length });
                       return;
                   }
                   else if (mount.overlay) {
                       let size = result.length;
                       explore(mount.tree, mount.overlay[0].from + from);
                       if (result.length > size)
                           return;
                   }
               }
               for (let i = 0; i < tree.children.length; i++) {
                   let ch = tree.children[i];
                   if (ch instanceof Tree)
                       explore(ch, tree.positions[i] + from);
               }
           };
           explore(syntaxTree(state), 0);
           return result;
       }
       /**
       Indicates whether this language allows nested languages. The
       default implementation returns true.
       */
       get allowsNesting() { return true; }
   }
   /**
   @internal
   */
   Language.setState = /*@__PURE__*/StateEffect.define();
   function languageDataFacetAt(state, pos, side) {
       let topLang = state.facet(language);
       if (!topLang)
           return null;
       let facet = topLang.data;
       if (topLang.allowsNesting) {
           for (let node = syntaxTree(state).topNode; node; node = node.enter(pos, side, IterMode.ExcludeBuffers))
               facet = node.type.prop(languageDataProp) || facet;
       }
       return facet;
   }
   /**
   A subclass of [`Language`](https://codemirror.net/6/docs/ref/#language.Language) for use with Lezer
   [LR parsers](https://lezer.codemirror.net/docs/ref#lr.LRParser)
   parsers.
   */
   class LRLanguage extends Language {
       constructor(data, parser) {
           super(data, parser);
           this.parser = parser;
       }
       /**
       Define a language from a parser.
       */
       static define(spec) {
           let data = defineLanguageFacet(spec.languageData);
           return new LRLanguage(data, spec.parser.configure({
               props: [languageDataProp.add(type => type.isTop ? data : undefined)]
           }));
       }
       /**
       Create a new instance of this language with a reconfigured
       version of its parser.
       */
       configure(options) {
           return new LRLanguage(this.data, this.parser.configure(options));
       }
       get allowsNesting() { return this.parser.hasWrappers(); }
   }
   /**
   Get the syntax tree for a state, which is the current (possibly
   incomplete) parse tree of the active
   [language](https://codemirror.net/6/docs/ref/#language.Language), or the empty tree if there is no
   language available.
   */
   function syntaxTree(state) {
       let field = state.field(Language.state, false);
       return field ? field.tree : Tree.empty;
   }
   // Lezer-style Input object for a Text document.
   class DocInput {
       constructor(doc, length = doc.length) {
           this.doc = doc;
           this.length = length;
           this.cursorPos = 0;
           this.string = "";
           this.cursor = doc.iter();
       }
       syncTo(pos) {
           this.string = this.cursor.next(pos - this.cursorPos).value;
           this.cursorPos = pos + this.string.length;
           return this.cursorPos - this.string.length;
       }
       chunk(pos) {
           this.syncTo(pos);
           return this.string;
       }
       get lineChunks() { return true; }
       read(from, to) {
           let stringStart = this.cursorPos - this.string.length;
           if (from < stringStart || to >= this.cursorPos)
               return this.doc.sliceString(from, to);
           else
               return this.string.slice(from - stringStart, to - stringStart);
       }
   }
   let currentContext = null;
   /**
   A parse context provided to parsers working on the editor content.
   */
   class ParseContext {
       constructor(parser, 
       /**
       The current editor state.
       */
       state, 
       /**
       Tree fragments that can be reused by incremental re-parses.
       */
       fragments = [], 
       /**
       @internal
       */
       tree, 
       /**
       @internal
       */
       treeLen, 
       /**
       The current editor viewport (or some overapproximation
       thereof). Intended to be used for opportunistically avoiding
       work (in which case
       [`skipUntilInView`](https://codemirror.net/6/docs/ref/#language.ParseContext.skipUntilInView)
       should be called to make sure the parser is restarted when the
       skipped region becomes visible).
       */
       viewport, 
       /**
       @internal
       */
       skipped, 
       /**
       This is where skipping parsers can register a promise that,
       when resolved, will schedule a new parse. It is cleared when
       the parse worker picks up the promise. @internal
       */
       scheduleOn) {
           this.parser = parser;
           this.state = state;
           this.fragments = fragments;
           this.tree = tree;
           this.treeLen = treeLen;
           this.viewport = viewport;
           this.skipped = skipped;
           this.scheduleOn = scheduleOn;
           this.parse = null;
           /**
           @internal
           */
           this.tempSkipped = [];
       }
       /**
       @internal
       */
       static create(parser, state, viewport) {
           return new ParseContext(parser, state, [], Tree.empty, 0, viewport, [], null);
       }
       startParse() {
           return this.parser.startParse(new DocInput(this.state.doc), this.fragments);
       }
       /**
       @internal
       */
       work(until, upto) {
           if (upto != null && upto >= this.state.doc.length)
               upto = undefined;
           if (this.tree != Tree.empty && this.isDone(upto !== null && upto !== void 0 ? upto : this.state.doc.length)) {
               this.takeTree();
               return true;
           }
           return this.withContext(() => {
               var _a;
               if (typeof until == "number") {
                   let endTime = Date.now() + until;
                   until = () => Date.now() > endTime;
               }
               if (!this.parse)
                   this.parse = this.startParse();
               if (upto != null && (this.parse.stoppedAt == null || this.parse.stoppedAt > upto) &&
                   upto < this.state.doc.length)
                   this.parse.stopAt(upto);
               for (;;) {
                   let done = this.parse.advance();
                   if (done) {
                       this.fragments = this.withoutTempSkipped(TreeFragment.addTree(done, this.fragments, this.parse.stoppedAt != null));
                       this.treeLen = (_a = this.parse.stoppedAt) !== null && _a !== void 0 ? _a : this.state.doc.length;
                       this.tree = done;
                       this.parse = null;
                       if (this.treeLen < (upto !== null && upto !== void 0 ? upto : this.state.doc.length))
                           this.parse = this.startParse();
                       else
                           return true;
                   }
                   if (until())
                       return false;
               }
           });
       }
       /**
       @internal
       */
       takeTree() {
           let pos, tree;
           if (this.parse && (pos = this.parse.parsedPos) >= this.treeLen) {
               if (this.parse.stoppedAt == null || this.parse.stoppedAt > pos)
                   this.parse.stopAt(pos);
               this.withContext(() => { while (!(tree = this.parse.advance())) { } });
               this.treeLen = pos;
               this.tree = tree;
               this.fragments = this.withoutTempSkipped(TreeFragment.addTree(this.tree, this.fragments, true));
               this.parse = null;
           }
       }
       withContext(f) {
           let prev = currentContext;
           currentContext = this;
           try {
               return f();
           }
           finally {
               currentContext = prev;
           }
       }
       withoutTempSkipped(fragments) {
           for (let r; r = this.tempSkipped.pop();)
               fragments = cutFragments(fragments, r.from, r.to);
           return fragments;
       }
       /**
       @internal
       */
       changes(changes, newState) {
           let { fragments, tree, treeLen, viewport, skipped } = this;
           this.takeTree();
           if (!changes.empty) {
               let ranges = [];
               changes.iterChangedRanges((fromA, toA, fromB, toB) => ranges.push({ fromA, toA, fromB, toB }));
               fragments = TreeFragment.applyChanges(fragments, ranges);
               tree = Tree.empty;
               treeLen = 0;
               viewport = { from: changes.mapPos(viewport.from, -1), to: changes.mapPos(viewport.to, 1) };
               if (this.skipped.length) {
                   skipped = [];
                   for (let r of this.skipped) {
                       let from = changes.mapPos(r.from, 1), to = changes.mapPos(r.to, -1);
                       if (from < to)
                           skipped.push({ from, to });
                   }
               }
           }
           return new ParseContext(this.parser, newState, fragments, tree, treeLen, viewport, skipped, this.scheduleOn);
       }
       /**
       @internal
       */
       updateViewport(viewport) {
           if (this.viewport.from == viewport.from && this.viewport.to == viewport.to)
               return false;
           this.viewport = viewport;
           let startLen = this.skipped.length;
           for (let i = 0; i < this.skipped.length; i++) {
               let { from, to } = this.skipped[i];
               if (from < viewport.to && to > viewport.from) {
                   this.fragments = cutFragments(this.fragments, from, to);
                   this.skipped.splice(i--, 1);
               }
           }
           if (this.skipped.length >= startLen)
               return false;
           this.reset();
           return true;
       }
       /**
       @internal
       */
       reset() {
           if (this.parse) {
               this.takeTree();
               this.parse = null;
           }
       }
       /**
       Notify the parse scheduler that the given region was skipped
       because it wasn't in view, and the parse should be restarted
       when it comes into view.
       */
       skipUntilInView(from, to) {
           this.skipped.push({ from, to });
       }
       /**
       Returns a parser intended to be used as placeholder when
       asynchronously loading a nested parser. It'll skip its input and
       mark it as not-really-parsed, so that the next update will parse
       it again.
       
       When `until` is given, a reparse will be scheduled when that
       promise resolves.
       */
       static getSkippingParser(until) {
           return new class extends Parser {
               createParse(input, fragments, ranges) {
                   let from = ranges[0].from, to = ranges[ranges.length - 1].to;
                   let parser = {
                       parsedPos: from,
                       advance() {
                           let cx = currentContext;
                           if (cx) {
                               for (let r of ranges)
                                   cx.tempSkipped.push(r);
                               if (until)
                                   cx.scheduleOn = cx.scheduleOn ? Promise.all([cx.scheduleOn, until]) : until;
                           }
                           this.parsedPos = to;
                           return new Tree(NodeType.none, [], [], to - from);
                       },
                       stoppedAt: null,
                       stopAt() { }
                   };
                   return parser;
               }
           };
       }
       /**
       @internal
       */
       isDone(upto) {
           upto = Math.min(upto, this.state.doc.length);
           let frags = this.fragments;
           return this.treeLen >= upto && frags.length && frags[0].from == 0 && frags[0].to >= upto;
       }
       /**
       Get the context for the current parse, or `null` if no editor
       parse is in progress.
       */
       static get() { return currentContext; }
   }
   function cutFragments(fragments, from, to) {
       return TreeFragment.applyChanges(fragments, [{ fromA: from, toA: to, fromB: from, toB: to }]);
   }
   class LanguageState {
       constructor(
       // A mutable parse state that is used to preserve work done during
       // the lifetime of a state when moving to the next state.
       context) {
           this.context = context;
           this.tree = context.tree;
       }
       apply(tr) {
           if (!tr.docChanged && this.tree == this.context.tree)
               return this;
           let newCx = this.context.changes(tr.changes, tr.state);
           // If the previous parse wasn't done, go forward only up to its
           // end position or the end of the viewport, to avoid slowing down
           // state updates with parse work beyond the viewport.
           let upto = this.context.treeLen == tr.startState.doc.length ? undefined
               : Math.max(tr.changes.mapPos(this.context.treeLen), newCx.viewport.to);
           if (!newCx.work(20 /* Apply */, upto))
               newCx.takeTree();
           return new LanguageState(newCx);
       }
       static init(state) {
           let vpTo = Math.min(3000 /* InitViewport */, state.doc.length);
           let parseState = ParseContext.create(state.facet(language).parser, state, { from: 0, to: vpTo });
           if (!parseState.work(20 /* Apply */, vpTo))
               parseState.takeTree();
           return new LanguageState(parseState);
       }
   }
   Language.state = /*@__PURE__*/StateField.define({
       create: LanguageState.init,
       update(value, tr) {
           for (let e of tr.effects)
               if (e.is(Language.setState))
                   return e.value;
           if (tr.startState.facet(language) != tr.state.facet(language))
               return LanguageState.init(tr.state);
           return value.apply(tr);
       }
   });
   let requestIdle = (callback) => {
       let timeout = setTimeout(() => callback(), 500 /* MaxPause */);
       return () => clearTimeout(timeout);
   };
   if (typeof requestIdleCallback != "undefined")
       requestIdle = (callback) => {
           let idle = -1, timeout = setTimeout(() => {
               idle = requestIdleCallback(callback, { timeout: 500 /* MaxPause */ - 100 /* MinPause */ });
           }, 100 /* MinPause */);
           return () => idle < 0 ? clearTimeout(timeout) : cancelIdleCallback(idle);
       };
   const isInputPending = typeof navigator != "undefined" && ((_a = navigator.scheduling) === null || _a === void 0 ? void 0 : _a.isInputPending)
       ? () => navigator.scheduling.isInputPending() : null;
   const parseWorker = /*@__PURE__*/ViewPlugin.fromClass(class ParseWorker {
       constructor(view) {
           this.view = view;
           this.working = null;
           this.workScheduled = 0;
           // End of the current time chunk
           this.chunkEnd = -1;
           // Milliseconds of budget left for this chunk
           this.chunkBudget = -1;
           this.work = this.work.bind(this);
           this.scheduleWork();
       }
       update(update) {
           let cx = this.view.state.field(Language.state).context;
           if (cx.updateViewport(update.view.viewport) || this.view.viewport.to > cx.treeLen)
               this.scheduleWork();
           if (update.docChanged) {
               if (this.view.hasFocus)
                   this.chunkBudget += 50 /* ChangeBonus */;
               this.scheduleWork();
           }
           this.checkAsyncSchedule(cx);
       }
       scheduleWork() {
           if (this.working)
               return;
           let { state } = this.view, field = state.field(Language.state);
           if (field.tree != field.context.tree || !field.context.isDone(state.doc.length))
               this.working = requestIdle(this.work);
       }
       work(deadline) {
           this.working = null;
           let now = Date.now();
           if (this.chunkEnd < now && (this.chunkEnd < 0 || this.view.hasFocus)) { // Start a new chunk
               this.chunkEnd = now + 30000 /* ChunkTime */;
               this.chunkBudget = 3000 /* ChunkBudget */;
           }
           if (this.chunkBudget <= 0)
               return; // No more budget
           let { state, viewport: { to: vpTo } } = this.view, field = state.field(Language.state);
           if (field.tree == field.context.tree && field.context.isDone(vpTo + 100000 /* MaxParseAhead */))
               return;
           let endTime = Date.now() + Math.min(this.chunkBudget, 100 /* Slice */, deadline && !isInputPending ? Math.max(25 /* MinSlice */, deadline.timeRemaining() - 5) : 1e9);
           let viewportFirst = field.context.treeLen < vpTo && state.doc.length > vpTo + 1000;
           let done = field.context.work(() => {
               return isInputPending && isInputPending() || Date.now() > endTime;
           }, vpTo + (viewportFirst ? 0 : 100000 /* MaxParseAhead */));
           this.chunkBudget -= Date.now() - now;
           if (done || this.chunkBudget <= 0) {
               field.context.takeTree();
               this.view.dispatch({ effects: Language.setState.of(new LanguageState(field.context)) });
           }
           if (this.chunkBudget > 0 && !(done && !viewportFirst))
               this.scheduleWork();
           this.checkAsyncSchedule(field.context);
       }
       checkAsyncSchedule(cx) {
           if (cx.scheduleOn) {
               this.workScheduled++;
               cx.scheduleOn
                   .then(() => this.scheduleWork())
                   .catch(err => logException(this.view.state, err))
                   .then(() => this.workScheduled--);
               cx.scheduleOn = null;
           }
       }
       destroy() {
           if (this.working)
               this.working();
       }
       isWorking() {
           return !!(this.working || this.workScheduled > 0);
       }
   }, {
       eventHandlers: { focus() { this.scheduleWork(); } }
   });
   /**
   The facet used to associate a language with an editor state. Used
   by `Language` object's `extension` property (so you don't need to
   manually wrap your languages in this). Can be used to access the
   current language on a state.
   */
   const language = /*@__PURE__*/Facet.define({
       combine(languages) { return languages.length ? languages[0] : null; },
       enables: [Language.state, parseWorker]
   });
   /**
   This class bundles a [language](https://codemirror.net/6/docs/ref/#language.Language) with an
   optional set of supporting extensions. Language packages are
   encouraged to export a function that optionally takes a
   configuration object and returns a `LanguageSupport` instance, as
   the main way for client code to use the package.
   */
   class LanguageSupport {
       /**
       Create a language support object.
       */
       constructor(
       /**
       The language object.
       */
       language, 
       /**
       An optional set of supporting extensions. When nesting a
       language in another language, the outer language is encouraged
       to include the supporting extensions for its inner languages
       in its own set of support extensions.
       */
       support = []) {
           this.language = language;
           this.support = support;
           this.extension = [language, support];
       }
   }

   /**
   Facet that defines a way to provide a function that computes the
   appropriate indentation depth at the start of a given line, or
   `null` to indicate no appropriate indentation could be determined.
   */
   const indentService = /*@__PURE__*/Facet.define();
   /**
   Facet for overriding the unit by which indentation happens.
   Should be a string consisting either entirely of spaces or
   entirely of tabs. When not set, this defaults to 2 spaces.
   */
   const indentUnit = /*@__PURE__*/Facet.define({
       combine: values => {
           if (!values.length)
               return "  ";
           if (!/^(?: +|\t+)$/.test(values[0]))
               throw new Error("Invalid indent unit: " + JSON.stringify(values[0]));
           return values[0];
       }
   });
   /**
   Return the _column width_ of an indent unit in the state.
   Determined by the [`indentUnit`](https://codemirror.net/6/docs/ref/#language.indentUnit)
   facet, and [`tabSize`](https://codemirror.net/6/docs/ref/#state.EditorState^tabSize) when that
   contains tabs.
   */
   function getIndentUnit(state) {
       let unit = state.facet(indentUnit);
       return unit.charCodeAt(0) == 9 ? state.tabSize * unit.length : unit.length;
   }
   /**
   Create an indentation string that covers columns 0 to `cols`.
   Will use tabs for as much of the columns as possible when the
   [`indentUnit`](https://codemirror.net/6/docs/ref/#language.indentUnit) facet contains
   tabs.
   */
   function indentString(state, cols) {
       let result = "", ts = state.tabSize;
       if (state.facet(indentUnit).charCodeAt(0) == 9)
           while (cols >= ts) {
               result += "\t";
               cols -= ts;
           }
       for (let i = 0; i < cols; i++)
           result += " ";
       return result;
   }
   /**
   Get the indentation at the given position. Will first consult any
   [indent services](https://codemirror.net/6/docs/ref/#language.indentService) that are registered,
   and if none of those return an indentation, this will check the
   syntax tree for the [indent node prop](https://codemirror.net/6/docs/ref/#language.indentNodeProp)
   and use that if found. Returns a number when an indentation could
   be determined, and null otherwise.
   */
   function getIndentation(context, pos) {
       if (context instanceof EditorState)
           context = new IndentContext(context);
       for (let service of context.state.facet(indentService)) {
           let result = service(context, pos);
           if (result != null)
               return result;
       }
       let tree = syntaxTree(context.state);
       return tree ? syntaxIndentation(context, tree, pos) : null;
   }
   /**
   Indentation contexts are used when calling [indentation
   services](https://codemirror.net/6/docs/ref/#language.indentService). They provide helper utilities
   useful in indentation logic, and can selectively override the
   indentation reported for some lines.
   */
   class IndentContext {
       /**
       Create an indent context.
       */
       constructor(
       /**
       The editor state.
       */
       state, 
       /**
       @internal
       */
       options = {}) {
           this.state = state;
           this.options = options;
           this.unit = getIndentUnit(state);
       }
       /**
       Get a description of the line at the given position, taking
       [simulated line
       breaks](https://codemirror.net/6/docs/ref/#language.IndentContext.constructor^options.simulateBreak)
       into account. If there is such a break at `pos`, the `bias`
       argument determines whether the part of the line line before or
       after the break is used.
       */
       lineAt(pos, bias = 1) {
           let line = this.state.doc.lineAt(pos);
           let { simulateBreak, simulateDoubleBreak } = this.options;
           if (simulateBreak != null && simulateBreak >= line.from && simulateBreak <= line.to) {
               if (simulateDoubleBreak && simulateBreak == pos)
                   return { text: "", from: pos };
               else if (bias < 0 ? simulateBreak < pos : simulateBreak <= pos)
                   return { text: line.text.slice(simulateBreak - line.from), from: simulateBreak };
               else
                   return { text: line.text.slice(0, simulateBreak - line.from), from: line.from };
           }
           return line;
       }
       /**
       Get the text directly after `pos`, either the entire line
       or the next 100 characters, whichever is shorter.
       */
       textAfterPos(pos, bias = 1) {
           if (this.options.simulateDoubleBreak && pos == this.options.simulateBreak)
               return "";
           let { text, from } = this.lineAt(pos, bias);
           return text.slice(pos - from, Math.min(text.length, pos + 100 - from));
       }
       /**
       Find the column for the given position.
       */
       column(pos, bias = 1) {
           let { text, from } = this.lineAt(pos, bias);
           let result = this.countColumn(text, pos - from);
           let override = this.options.overrideIndentation ? this.options.overrideIndentation(from) : -1;
           if (override > -1)
               result += override - this.countColumn(text, text.search(/\S|$/));
           return result;
       }
       /**
       Find the column position (taking tabs into account) of the given
       position in the given string.
       */
       countColumn(line, pos = line.length) {
           return countColumn(line, this.state.tabSize, pos);
       }
       /**
       Find the indentation column of the line at the given point.
       */
       lineIndent(pos, bias = 1) {
           let { text, from } = this.lineAt(pos, bias);
           let override = this.options.overrideIndentation;
           if (override) {
               let overriden = override(from);
               if (overriden > -1)
                   return overriden;
           }
           return this.countColumn(text, text.search(/\S|$/));
       }
       /**
       Returns the [simulated line
       break](https://codemirror.net/6/docs/ref/#language.IndentContext.constructor^options.simulateBreak)
       for this context, if any.
       */
       get simulatedBreak() {
           return this.options.simulateBreak || null;
       }
   }
   /**
   A syntax tree node prop used to associate indentation strategies
   with node types. Such a strategy is a function from an indentation
   context to a column number or null, where null indicates that no
   definitive indentation can be determined.
   */
   const indentNodeProp = /*@__PURE__*/new NodeProp();
   // Compute the indentation for a given position from the syntax tree.
   function syntaxIndentation(cx, ast, pos) {
       return indentFrom(ast.resolveInner(pos).enterUnfinishedNodesBefore(pos), pos, cx);
   }
   function ignoreClosed(cx) {
       return cx.pos == cx.options.simulateBreak && cx.options.simulateDoubleBreak;
   }
   function indentStrategy(tree) {
       let strategy = tree.type.prop(indentNodeProp);
       if (strategy)
           return strategy;
       let first = tree.firstChild, close;
       if (first && (close = first.type.prop(NodeProp.closedBy))) {
           let last = tree.lastChild, closed = last && close.indexOf(last.name) > -1;
           return cx => delimitedStrategy(cx, true, 1, undefined, closed && !ignoreClosed(cx) ? last.from : undefined);
       }
       return tree.parent == null ? topIndent : null;
   }
   function indentFrom(node, pos, base) {
       for (; node; node = node.parent) {
           let strategy = indentStrategy(node);
           if (strategy)
               return strategy(TreeIndentContext.create(base, pos, node));
       }
       return null;
   }
   function topIndent() { return 0; }
   /**
   Objects of this type provide context information and helper
   methods to indentation functions registered on syntax nodes.
   */
   class TreeIndentContext extends IndentContext {
       constructor(base, 
       /**
       The position at which indentation is being computed.
       */
       pos, 
       /**
       The syntax tree node to which the indentation strategy
       applies.
       */
       node) {
           super(base.state, base.options);
           this.base = base;
           this.pos = pos;
           this.node = node;
       }
       /**
       @internal
       */
       static create(base, pos, node) {
           return new TreeIndentContext(base, pos, node);
       }
       /**
       Get the text directly after `this.pos`, either the entire line
       or the next 100 characters, whichever is shorter.
       */
       get textAfter() {
           return this.textAfterPos(this.pos);
       }
       /**
       Get the indentation at the reference line for `this.node`, which
       is the line on which it starts, unless there is a node that is
       _not_ a parent of this node covering the start of that line. If
       so, the line at the start of that node is tried, again skipping
       on if it is covered by another such node.
       */
       get baseIndent() {
           let line = this.state.doc.lineAt(this.node.from);
           // Skip line starts that are covered by a sibling (or cousin, etc)
           for (;;) {
               let atBreak = this.node.resolve(line.from);
               while (atBreak.parent && atBreak.parent.from == atBreak.from)
                   atBreak = atBreak.parent;
               if (isParent(atBreak, this.node))
                   break;
               line = this.state.doc.lineAt(atBreak.from);
           }
           return this.lineIndent(line.from);
       }
       /**
       Continue looking for indentations in the node's parent nodes,
       and return the result of that.
       */
       continue() {
           let parent = this.node.parent;
           return parent ? indentFrom(parent, this.pos, this.base) : 0;
       }
   }
   function isParent(parent, of) {
       for (let cur = of; cur; cur = cur.parent)
           if (parent == cur)
               return true;
       return false;
   }
   // Check whether a delimited node is aligned (meaning there are
   // non-skipped nodes on the same line as the opening delimiter). And
   // if so, return the opening token.
   function bracketedAligned(context) {
       let tree = context.node;
       let openToken = tree.childAfter(tree.from), last = tree.lastChild;
       if (!openToken)
           return null;
       let sim = context.options.simulateBreak;
       let openLine = context.state.doc.lineAt(openToken.from);
       let lineEnd = sim == null || sim <= openLine.from ? openLine.to : Math.min(openLine.to, sim);
       for (let pos = openToken.to;;) {
           let next = tree.childAfter(pos);
           if (!next || next == last)
               return null;
           if (!next.type.isSkipped)
               return next.from < lineEnd ? openToken : null;
           pos = next.to;
       }
   }
   /**
   An indentation strategy for delimited (usually bracketed) nodes.
   Will, by default, indent one unit more than the parent's base
   indent unless the line starts with a closing token. When `align`
   is true and there are non-skipped nodes on the node's opening
   line, the content of the node will be aligned with the end of the
   opening node, like this:

       foo(bar,
           baz)
   */
   function delimitedIndent({ closing, align = true, units = 1 }) {
       return (context) => delimitedStrategy(context, align, units, closing);
   }
   function delimitedStrategy(context, align, units, closing, closedAt) {
       let after = context.textAfter, space = after.match(/^\s*/)[0].length;
       let closed = closing && after.slice(space, space + closing.length) == closing || closedAt == context.pos + space;
       let aligned = align ? bracketedAligned(context) : null;
       if (aligned)
           return closed ? context.column(aligned.from) : context.column(aligned.to);
       return context.baseIndent + (closed ? 0 : context.unit * units);
   }
   /**
   An indentation strategy that aligns a node's content to its base
   indentation.
   */
   const flatIndent = (context) => context.baseIndent;
   /**
   Creates an indentation strategy that, by default, indents
   continued lines one unit more than the node's base indentation.
   You can provide `except` to prevent indentation of lines that
   match a pattern (for example `/^else\b/` in `if`/`else`
   constructs), and you can change the amount of units used with the
   `units` option.
   */
   function continuedIndent({ except, units = 1 } = {}) {
       return (context) => {
           let matchExcept = except && except.test(context.textAfter);
           return context.baseIndent + (matchExcept ? 0 : units * context.unit);
       };
   }
   /**
   This node prop is used to associate folding information with
   syntax node types. Given a syntax node, it should check whether
   that tree is foldable and return the range that can be collapsed
   when it is.
   */
   const foldNodeProp = /*@__PURE__*/new NodeProp();
   /**
   [Fold](https://codemirror.net/6/docs/ref/#language.foldNodeProp) function that folds everything but
   the first and the last child of a syntax node. Useful for nodes
   that start and end with delimiters.
   */
   function foldInside(node) {
       let first = node.firstChild, last = node.lastChild;
       return first && first.to < last.from ? { from: first.to, to: last.type.isError ? node.to : last.from } : null;
   }

   /**
   A highlight style associates CSS styles with higlighting
   [tags](https://lezer.codemirror.net/docs/ref#highlight.Tag).
   */
   class HighlightStyle {
       constructor(spec, options) {
           let modSpec;
           function def(spec) {
               let cls = StyleModule.newName();
               (modSpec || (modSpec = Object.create(null)))["." + cls] = spec;
               return cls;
           }
           const all = typeof options.all == "string" ? options.all : options.all ? def(options.all) : undefined;
           const scopeOpt = options.scope;
           this.scope = scopeOpt instanceof Language ? (type) => type.prop(languageDataProp) == scopeOpt.data
               : scopeOpt ? (type) => type == scopeOpt : undefined;
           this.style = tagHighlighter(spec.map(style => ({
               tag: style.tag,
               class: style.class || def(Object.assign({}, style, { tag: null }))
           })), {
               all,
           }).style;
           this.module = modSpec ? new StyleModule(modSpec) : null;
           this.themeType = options.themeType;
       }
       /**
       Create a highlighter style that associates the given styles to
       the given tags. The specs must be objects that hold a style tag
       or array of tags in their `tag` property, and either a single
       `class` property providing a static CSS class (for highlighter
       that rely on external styling), or a
       [`style-mod`](https://github.com/marijnh/style-mod#documentation)-style
       set of CSS properties (which define the styling for those tags).
       
       The CSS rules created for a highlighter will be emitted in the
       order of the spec's properties. That means that for elements that
       have multiple tags associated with them, styles defined further
       down in the list will have a higher CSS precedence than styles
       defined earlier.
       */
       static define(specs, options) {
           return new HighlightStyle(specs, options || {});
       }
   }
   const highlighterFacet = /*@__PURE__*/Facet.define();
   const fallbackHighlighter = /*@__PURE__*/Facet.define({
       combine(values) { return values.length ? [values[0]] : null; }
   });
   function getHighlighters(state) {
       let main = state.facet(highlighterFacet);
       return main.length ? main : state.facet(fallbackHighlighter);
   }
   /**
   Wrap a highlighter in an editor extension that uses it to apply
   syntax highlighting to the editor content.

   When multiple (non-fallback) styles are provided, the styling
   applied is the union of the classes they emit.
   */
   function syntaxHighlighting(highlighter, options) {
       let ext = [treeHighlighter], themeType;
       if (highlighter instanceof HighlightStyle) {
           if (highlighter.module)
               ext.push(EditorView.styleModule.of(highlighter.module));
           themeType = highlighter.themeType;
       }
       if (options === null || options === void 0 ? void 0 : options.fallback)
           ext.push(fallbackHighlighter.of(highlighter));
       else if (themeType)
           ext.push(highlighterFacet.computeN([EditorView.darkTheme], state => {
               return state.facet(EditorView.darkTheme) == (themeType == "dark") ? [highlighter] : [];
           }));
       else
           ext.push(highlighterFacet.of(highlighter));
       return ext;
   }
   class TreeHighlighter {
       constructor(view) {
           this.markCache = Object.create(null);
           this.tree = syntaxTree(view.state);
           this.decorations = this.buildDeco(view, getHighlighters(view.state));
       }
       update(update) {
           let tree = syntaxTree(update.state), highlighters = getHighlighters(update.state);
           let styleChange = highlighters != getHighlighters(update.startState);
           if (tree.length < update.view.viewport.to && !styleChange && tree.type == this.tree.type) {
               this.decorations = this.decorations.map(update.changes);
           }
           else if (tree != this.tree || update.viewportChanged || styleChange) {
               this.tree = tree;
               this.decorations = this.buildDeco(update.view, highlighters);
           }
       }
       buildDeco(view, highlighters) {
           if (!highlighters || !this.tree.length)
               return Decoration.none;
           let builder = new RangeSetBuilder();
           for (let { from, to } of view.visibleRanges) {
               highlightTree(this.tree, highlighters, (from, to, style) => {
                   builder.add(from, to, this.markCache[style] || (this.markCache[style] = Decoration.mark({ class: style })));
               }, from, to);
           }
           return builder.finish();
       }
   }
   const treeHighlighter = /*@__PURE__*/Prec.high(/*@__PURE__*/ViewPlugin.fromClass(TreeHighlighter, {
       decorations: v => v.decorations
   }));
   /**
   A default highlight style (works well with light themes).
   */
   const defaultHighlightStyle = /*@__PURE__*/HighlightStyle.define([
       { tag: tags.meta,
           color: "#7a757a" },
       { tag: tags.link,
           textDecoration: "underline" },
       { tag: tags.heading,
           textDecoration: "underline",
           fontWeight: "bold" },
       { tag: tags.emphasis,
           fontStyle: "italic" },
       { tag: tags.strong,
           fontWeight: "bold" },
       { tag: tags.strikethrough,
           textDecoration: "line-through" },
       { tag: tags.keyword,
           color: "#708" },
       { tag: [tags.atom, tags.bool, tags.url, tags.contentSeparator, tags.labelName],
           color: "#219" },
       { tag: [tags.literal, tags.inserted],
           color: "#164" },
       { tag: [tags.string, tags.deleted],
           color: "#a11" },
       { tag: [tags.regexp, tags.escape, /*@__PURE__*/tags.special(tags.string)],
           color: "#e40" },
       { tag: /*@__PURE__*/tags.definition(tags.variableName),
           color: "#00f" },
       { tag: /*@__PURE__*/tags.local(tags.variableName),
           color: "#30a" },
       { tag: [tags.typeName, tags.namespace],
           color: "#085" },
       { tag: tags.className,
           color: "#167" },
       { tag: [/*@__PURE__*/tags.special(tags.variableName), tags.macroName],
           color: "#256" },
       { tag: /*@__PURE__*/tags.definition(tags.propertyName),
           color: "#00c" },
       { tag: tags.comment,
           color: "#940" },
       { tag: tags.invalid,
           color: "#f00" }
   ]);
   const DefaultScanDist = 10000, DefaultBrackets = "()[]{}";
   function matchingNodes(node, dir, brackets) {
       let byProp = node.prop(dir < 0 ? NodeProp.openedBy : NodeProp.closedBy);
       if (byProp)
           return byProp;
       if (node.name.length == 1) {
           let index = brackets.indexOf(node.name);
           if (index > -1 && index % 2 == (dir < 0 ? 1 : 0))
               return [brackets[index + dir]];
       }
       return null;
   }
   /**
   Find the matching bracket for the token at `pos`, scanning
   direction `dir`. Only the `brackets` and `maxScanDistance`
   properties are used from `config`, if given. Returns null if no
   bracket was found at `pos`, or a match result otherwise.
   */
   function matchBrackets(state, pos, dir, config = {}) {
       let maxScanDistance = config.maxScanDistance || DefaultScanDist, brackets = config.brackets || DefaultBrackets;
       let tree = syntaxTree(state), node = tree.resolveInner(pos, dir);
       for (let cur = node; cur; cur = cur.parent) {
           let matches = matchingNodes(cur.type, dir, brackets);
           if (matches && cur.from < cur.to)
               return matchMarkedBrackets(state, pos, dir, cur, matches, brackets);
       }
       return matchPlainBrackets(state, pos, dir, tree, node.type, maxScanDistance, brackets);
   }
   function matchMarkedBrackets(_state, _pos, dir, token, matching, brackets) {
       let parent = token.parent, firstToken = { from: token.from, to: token.to };
       let depth = 0, cursor = parent === null || parent === void 0 ? void 0 : parent.cursor();
       if (cursor && (dir < 0 ? cursor.childBefore(token.from) : cursor.childAfter(token.to)))
           do {
               if (dir < 0 ? cursor.to <= token.from : cursor.from >= token.to) {
                   if (depth == 0 && matching.indexOf(cursor.type.name) > -1 && cursor.from < cursor.to) {
                       return { start: firstToken, end: { from: cursor.from, to: cursor.to }, matched: true };
                   }
                   else if (matchingNodes(cursor.type, dir, brackets)) {
                       depth++;
                   }
                   else if (matchingNodes(cursor.type, -dir, brackets)) {
                       if (depth == 0)
                           return {
                               start: firstToken,
                               end: cursor.from == cursor.to ? undefined : { from: cursor.from, to: cursor.to },
                               matched: false
                           };
                       depth--;
                   }
               }
           } while (dir < 0 ? cursor.prevSibling() : cursor.nextSibling());
       return { start: firstToken, matched: false };
   }
   function matchPlainBrackets(state, pos, dir, tree, tokenType, maxScanDistance, brackets) {
       let startCh = dir < 0 ? state.sliceDoc(pos - 1, pos) : state.sliceDoc(pos, pos + 1);
       let bracket = brackets.indexOf(startCh);
       if (bracket < 0 || (bracket % 2 == 0) != (dir > 0))
           return null;
       let startToken = { from: dir < 0 ? pos - 1 : pos, to: dir > 0 ? pos + 1 : pos };
       let iter = state.doc.iterRange(pos, dir > 0 ? state.doc.length : 0), depth = 0;
       for (let distance = 0; !(iter.next()).done && distance <= maxScanDistance;) {
           let text = iter.value;
           if (dir < 0)
               distance += text.length;
           let basePos = pos + distance * dir;
           for (let pos = dir > 0 ? 0 : text.length - 1, end = dir > 0 ? text.length : -1; pos != end; pos += dir) {
               let found = brackets.indexOf(text[pos]);
               if (found < 0 || tree.resolveInner(basePos + pos, 1).type != tokenType)
                   continue;
               if ((found % 2 == 0) == (dir > 0)) {
                   depth++;
               }
               else if (depth == 1) { // Closing
                   return { start: startToken, end: { from: basePos + pos, to: basePos + pos + 1 }, matched: (found >> 1) == (bracket >> 1) };
               }
               else {
                   depth--;
               }
           }
           if (dir > 0)
               distance += text.length;
       }
       return iter.done ? { start: startToken, matched: false } : null;
   }
   const noTokens = /*@__PURE__*/Object.create(null);
   const typeArray = [NodeType.none];
   const warned = [];
   const defaultTable = /*@__PURE__*/Object.create(null);
   for (let [legacyName, name] of [
       ["variable", "variableName"],
       ["variable-2", "variableName.special"],
       ["string-2", "string.special"],
       ["def", "variableName.definition"],
       ["tag", "tagName"],
       ["attribute", "attributeName"],
       ["type", "typeName"],
       ["builtin", "variableName.standard"],
       ["qualifier", "modifier"],
       ["error", "invalid"],
       ["header", "heading"],
       ["property", "propertyName"]
   ])
       defaultTable[legacyName] = /*@__PURE__*/createTokenType(noTokens, name);
   function warnForPart(part, msg) {
       if (warned.indexOf(part) > -1)
           return;
       warned.push(part);
       console.warn(msg);
   }
   function createTokenType(extra, tagStr) {
       let tag = null;
       for (let part of tagStr.split(".")) {
           let value = (extra[part] || tags[part]);
           if (!value) {
               warnForPart(part, `Unknown highlighting tag ${part}`);
           }
           else if (typeof value == "function") {
               if (!tag)
                   warnForPart(part, `Modifier ${part} used at start of tag`);
               else
                   tag = value(tag);
           }
           else {
               if (tag)
                   warnForPart(part, `Tag ${part} used as modifier`);
               else
                   tag = value;
           }
       }
       if (!tag)
           return 0;
       let name = tagStr.replace(/ /g, "_"), type = NodeType.define({
           id: typeArray.length,
           name,
           props: [styleTags({ [name]: tag })]
       });
       typeArray.push(type);
       return type.id;
   }

   /**
   Comment or uncomment the current selection. Will use line comments
   if available, otherwise falling back to block comments.
   */
   const toggleComment = target => {
       let config = getConfig(target.state);
       return config.line ? toggleLineComment(target) : config.block ? toggleBlockCommentByLine(target) : false;
   };
   function command(f, option) {
       return ({ state, dispatch }) => {
           if (state.readOnly)
               return false;
           let tr = f(option, state);
           if (!tr)
               return false;
           dispatch(state.update(tr));
           return true;
       };
   }
   /**
   Comment or uncomment the current selection using line comments.
   The line comment syntax is taken from the
   [`commentTokens`](https://codemirror.net/6/docs/ref/#commands.CommentTokens) [language
   data](https://codemirror.net/6/docs/ref/#state.EditorState.languageDataAt).
   */
   const toggleLineComment = /*@__PURE__*/command(changeLineComment, 0 /* Toggle */);
   /**
   Comment or uncomment the current selection using block comments.
   The block comment syntax is taken from the
   [`commentTokens`](https://codemirror.net/6/docs/ref/#commands.CommentTokens) [language
   data](https://codemirror.net/6/docs/ref/#state.EditorState.languageDataAt).
   */
   const toggleBlockComment = /*@__PURE__*/command(changeBlockComment, 0 /* Toggle */);
   /**
   Comment or uncomment the lines around the current selection using
   block comments.
   */
   const toggleBlockCommentByLine = /*@__PURE__*/command((o, s) => changeBlockComment(o, s, selectedLineRanges(s)), 0 /* Toggle */);
   function getConfig(state, pos = state.selection.main.head) {
       let data = state.languageDataAt("commentTokens", pos);
       return data.length ? data[0] : {};
   }
   const SearchMargin = 50;
   /**
   Determines if the given range is block-commented in the given
   state.
   */
   function findBlockComment(state, { open, close }, from, to) {
       let textBefore = state.sliceDoc(from - SearchMargin, from);
       let textAfter = state.sliceDoc(to, to + SearchMargin);
       let spaceBefore = /\s*$/.exec(textBefore)[0].length, spaceAfter = /^\s*/.exec(textAfter)[0].length;
       let beforeOff = textBefore.length - spaceBefore;
       if (textBefore.slice(beforeOff - open.length, beforeOff) == open &&
           textAfter.slice(spaceAfter, spaceAfter + close.length) == close) {
           return { open: { pos: from - spaceBefore, margin: spaceBefore && 1 },
               close: { pos: to + spaceAfter, margin: spaceAfter && 1 } };
       }
       let startText, endText;
       if (to - from <= 2 * SearchMargin) {
           startText = endText = state.sliceDoc(from, to);
       }
       else {
           startText = state.sliceDoc(from, from + SearchMargin);
           endText = state.sliceDoc(to - SearchMargin, to);
       }
       let startSpace = /^\s*/.exec(startText)[0].length, endSpace = /\s*$/.exec(endText)[0].length;
       let endOff = endText.length - endSpace - close.length;
       if (startText.slice(startSpace, startSpace + open.length) == open &&
           endText.slice(endOff, endOff + close.length) == close) {
           return { open: { pos: from + startSpace + open.length,
                   margin: /\s/.test(startText.charAt(startSpace + open.length)) ? 1 : 0 },
               close: { pos: to - endSpace - close.length,
                   margin: /\s/.test(endText.charAt(endOff - 1)) ? 1 : 0 } };
       }
       return null;
   }
   function selectedLineRanges(state) {
       let ranges = [];
       for (let r of state.selection.ranges) {
           let fromLine = state.doc.lineAt(r.from);
           let toLine = r.to <= fromLine.to ? fromLine : state.doc.lineAt(r.to);
           let last = ranges.length - 1;
           if (last >= 0 && ranges[last].to > fromLine.from)
               ranges[last].to = toLine.to;
           else
               ranges.push({ from: fromLine.from, to: toLine.to });
       }
       return ranges;
   }
   // Performs toggle, comment and uncomment of block comments in
   // languages that support them.
   function changeBlockComment(option, state, ranges = state.selection.ranges) {
       let tokens = ranges.map(r => getConfig(state, r.from).block);
       if (!tokens.every(c => c))
           return null;
       let comments = ranges.map((r, i) => findBlockComment(state, tokens[i], r.from, r.to));
       if (option != 2 /* Uncomment */ && !comments.every(c => c)) {
           return { changes: state.changes(ranges.map((range, i) => {
                   if (comments[i])
                       return [];
                   return [{ from: range.from, insert: tokens[i].open + " " }, { from: range.to, insert: " " + tokens[i].close }];
               })) };
       }
       else if (option != 1 /* Comment */ && comments.some(c => c)) {
           let changes = [];
           for (let i = 0, comment; i < comments.length; i++)
               if (comment = comments[i]) {
                   let token = tokens[i], { open, close } = comment;
                   changes.push({ from: open.pos - token.open.length, to: open.pos + open.margin }, { from: close.pos - close.margin, to: close.pos + token.close.length });
               }
           return { changes };
       }
       return null;
   }
   // Performs toggle, comment and uncomment of line comments.
   function changeLineComment(option, state, ranges = state.selection.ranges) {
       let lines = [];
       let prevLine = -1;
       for (let { from, to } of ranges) {
           let startI = lines.length, minIndent = 1e9;
           for (let pos = from; pos <= to;) {
               let line = state.doc.lineAt(pos);
               if (line.from > prevLine && (from == to || to > line.from)) {
                   prevLine = line.from;
                   let token = getConfig(state, pos).line;
                   if (!token)
                       continue;
                   let indent = /^\s*/.exec(line.text)[0].length;
                   let empty = indent == line.length;
                   let comment = line.text.slice(indent, indent + token.length) == token ? indent : -1;
                   if (indent < line.text.length && indent < minIndent)
                       minIndent = indent;
                   lines.push({ line, comment, token, indent, empty, single: false });
               }
               pos = line.to + 1;
           }
           if (minIndent < 1e9)
               for (let i = startI; i < lines.length; i++)
                   if (lines[i].indent < lines[i].line.text.length)
                       lines[i].indent = minIndent;
           if (lines.length == startI + 1)
               lines[startI].single = true;
       }
       if (option != 2 /* Uncomment */ && lines.some(l => l.comment < 0 && (!l.empty || l.single))) {
           let changes = [];
           for (let { line, token, indent, empty, single } of lines)
               if (single || !empty)
                   changes.push({ from: line.from + indent, insert: token + " " });
           let changeSet = state.changes(changes);
           return { changes: changeSet, selection: state.selection.map(changeSet, 1) };
       }
       else if (option != 1 /* Comment */ && lines.some(l => l.comment >= 0)) {
           let changes = [];
           for (let { line, comment, token } of lines)
               if (comment >= 0) {
                   let from = line.from + comment, to = from + token.length;
                   if (line.text[to - line.from] == " ")
                       to++;
                   changes.push({ from, to });
               }
           return { changes };
       }
       return null;
   }

   const fromHistory = /*@__PURE__*/Annotation.define();
   /**
   Transaction annotation that will prevent that transaction from
   being combined with other transactions in the undo history. Given
   `"before"`, it'll prevent merging with previous transactions. With
   `"after"`, subsequent transactions won't be combined with this
   one. With `"full"`, the transaction is isolated on both sides.
   */
   const isolateHistory = /*@__PURE__*/Annotation.define();
   /**
   This facet provides a way to register functions that, given a
   transaction, provide a set of effects that the history should
   store when inverting the transaction. This can be used to
   integrate some kinds of effects in the history, so that they can
   be undone (and redone again).
   */
   const invertedEffects = /*@__PURE__*/Facet.define();
   const historyConfig = /*@__PURE__*/Facet.define({
       combine(configs) {
           return combineConfig(configs, {
               minDepth: 100,
               newGroupDelay: 500
           }, { minDepth: Math.max, newGroupDelay: Math.min });
       }
   });
   function changeEnd(changes) {
       let end = 0;
       changes.iterChangedRanges((_, to) => end = to);
       return end;
   }
   const historyField_ = /*@__PURE__*/StateField.define({
       create() {
           return HistoryState.empty;
       },
       update(state, tr) {
           let config = tr.state.facet(historyConfig);
           let fromHist = tr.annotation(fromHistory);
           if (fromHist) {
               let selection = tr.docChanged ? EditorSelection.single(changeEnd(tr.changes)) : undefined;
               let item = HistEvent.fromTransaction(tr, selection), from = fromHist.side;
               let other = from == 0 /* Done */ ? state.undone : state.done;
               if (item)
                   other = updateBranch(other, other.length, config.minDepth, item);
               else
                   other = addSelection(other, tr.startState.selection);
               return new HistoryState(from == 0 /* Done */ ? fromHist.rest : other, from == 0 /* Done */ ? other : fromHist.rest);
           }
           let isolate = tr.annotation(isolateHistory);
           if (isolate == "full" || isolate == "before")
               state = state.isolate();
           if (tr.annotation(Transaction.addToHistory) === false)
               return !tr.changes.empty ? state.addMapping(tr.changes.desc) : state;
           let event = HistEvent.fromTransaction(tr);
           let time = tr.annotation(Transaction.time), userEvent = tr.annotation(Transaction.userEvent);
           if (event)
               state = state.addChanges(event, time, userEvent, config.newGroupDelay, config.minDepth);
           else if (tr.selection)
               state = state.addSelection(tr.startState.selection, time, userEvent, config.newGroupDelay);
           if (isolate == "full" || isolate == "after")
               state = state.isolate();
           return state;
       },
       toJSON(value) {
           return { done: value.done.map(e => e.toJSON()), undone: value.undone.map(e => e.toJSON()) };
       },
       fromJSON(json) {
           return new HistoryState(json.done.map(HistEvent.fromJSON), json.undone.map(HistEvent.fromJSON));
       }
   });
   /**
   Create a history extension with the given configuration.
   */
   function history(config = {}) {
       return [
           historyField_,
           historyConfig.of(config),
           EditorView.domEventHandlers({
               beforeinput(e, view) {
                   let command = e.inputType == "historyUndo" ? undo : e.inputType == "historyRedo" ? redo : null;
                   if (!command)
                       return false;
                   e.preventDefault();
                   return command(view);
               }
           })
       ];
   }
   function cmd(side, selection) {
       return function ({ state, dispatch }) {
           if (!selection && state.readOnly)
               return false;
           let historyState = state.field(historyField_, false);
           if (!historyState)
               return false;
           let tr = historyState.pop(side, state, selection);
           if (!tr)
               return false;
           dispatch(tr);
           return true;
       };
   }
   /**
   Undo a single group of history events. Returns false if no group
   was available.
   */
   const undo = /*@__PURE__*/cmd(0 /* Done */, false);
   /**
   Redo a group of history events. Returns false if no group was
   available.
   */
   const redo = /*@__PURE__*/cmd(1 /* Undone */, false);
   /**
   Undo a change or selection change.
   */
   const undoSelection = /*@__PURE__*/cmd(0 /* Done */, true);
   /**
   Redo a change or selection change.
   */
   const redoSelection = /*@__PURE__*/cmd(1 /* Undone */, true);
   // History events store groups of changes or effects that need to be
   // undone/redone together.
   class HistEvent {
       constructor(
       // The changes in this event. Normal events hold at least one
       // change or effect. But it may be necessary to store selection
       // events before the first change, in which case a special type of
       // instance is created which doesn't hold any changes, with
       // changes == startSelection == undefined
       changes, 
       // The effects associated with this event
       effects, 
       // Accumulated mapping (from addToHistory==false) that should be
       // applied to events below this one.
       mapped, 
       // The selection before this event
       startSelection, 
       // Stores selection changes after this event, to be used for
       // selection undo/redo.
       selectionsAfter) {
           this.changes = changes;
           this.effects = effects;
           this.mapped = mapped;
           this.startSelection = startSelection;
           this.selectionsAfter = selectionsAfter;
       }
       setSelAfter(after) {
           return new HistEvent(this.changes, this.effects, this.mapped, this.startSelection, after);
       }
       toJSON() {
           var _a, _b, _c;
           return {
               changes: (_a = this.changes) === null || _a === void 0 ? void 0 : _a.toJSON(),
               mapped: (_b = this.mapped) === null || _b === void 0 ? void 0 : _b.toJSON(),
               startSelection: (_c = this.startSelection) === null || _c === void 0 ? void 0 : _c.toJSON(),
               selectionsAfter: this.selectionsAfter.map(s => s.toJSON())
           };
       }
       static fromJSON(json) {
           return new HistEvent(json.changes && ChangeSet.fromJSON(json.changes), [], json.mapped && ChangeDesc.fromJSON(json.mapped), json.startSelection && EditorSelection.fromJSON(json.startSelection), json.selectionsAfter.map(EditorSelection.fromJSON));
       }
       // This does not check `addToHistory` and such, it assumes the
       // transaction needs to be converted to an item. Returns null when
       // there are no changes or effects in the transaction.
       static fromTransaction(tr, selection) {
           let effects = none;
           for (let invert of tr.startState.facet(invertedEffects)) {
               let result = invert(tr);
               if (result.length)
                   effects = effects.concat(result);
           }
           if (!effects.length && tr.changes.empty)
               return null;
           return new HistEvent(tr.changes.invert(tr.startState.doc), effects, undefined, selection || tr.startState.selection, none);
       }
       static selection(selections) {
           return new HistEvent(undefined, none, undefined, undefined, selections);
       }
   }
   function updateBranch(branch, to, maxLen, newEvent) {
       let start = to + 1 > maxLen + 20 ? to - maxLen - 1 : 0;
       let newBranch = branch.slice(start, to);
       newBranch.push(newEvent);
       return newBranch;
   }
   function isAdjacent(a, b) {
       let ranges = [], isAdjacent = false;
       a.iterChangedRanges((f, t) => ranges.push(f, t));
       b.iterChangedRanges((_f, _t, f, t) => {
           for (let i = 0; i < ranges.length;) {
               let from = ranges[i++], to = ranges[i++];
               if (t >= from && f <= to)
                   isAdjacent = true;
           }
       });
       return isAdjacent;
   }
   function eqSelectionShape(a, b) {
       return a.ranges.length == b.ranges.length &&
           a.ranges.filter((r, i) => r.empty != b.ranges[i].empty).length === 0;
   }
   function conc(a, b) {
       return !a.length ? b : !b.length ? a : a.concat(b);
   }
   const none = [];
   const MaxSelectionsPerEvent = 200;
   function addSelection(branch, selection) {
       if (!branch.length) {
           return [HistEvent.selection([selection])];
       }
       else {
           let lastEvent = branch[branch.length - 1];
           let sels = lastEvent.selectionsAfter.slice(Math.max(0, lastEvent.selectionsAfter.length - MaxSelectionsPerEvent));
           if (sels.length && sels[sels.length - 1].eq(selection))
               return branch;
           sels.push(selection);
           return updateBranch(branch, branch.length - 1, 1e9, lastEvent.setSelAfter(sels));
       }
   }
   // Assumes the top item has one or more selectionAfter values
   function popSelection(branch) {
       let last = branch[branch.length - 1];
       let newBranch = branch.slice();
       newBranch[branch.length - 1] = last.setSelAfter(last.selectionsAfter.slice(0, last.selectionsAfter.length - 1));
       return newBranch;
   }
   // Add a mapping to the top event in the given branch. If this maps
   // away all the changes and effects in that item, drop it and
   // propagate the mapping to the next item.
   function addMappingToBranch(branch, mapping) {
       if (!branch.length)
           return branch;
       let length = branch.length, selections = none;
       while (length) {
           let event = mapEvent(branch[length - 1], mapping, selections);
           if (event.changes && !event.changes.empty || event.effects.length) { // Event survived mapping
               let result = branch.slice(0, length);
               result[length - 1] = event;
               return result;
           }
           else { // Drop this event, since there's no changes or effects left
               mapping = event.mapped;
               length--;
               selections = event.selectionsAfter;
           }
       }
       return selections.length ? [HistEvent.selection(selections)] : none;
   }
   function mapEvent(event, mapping, extraSelections) {
       let selections = conc(event.selectionsAfter.length ? event.selectionsAfter.map(s => s.map(mapping)) : none, extraSelections);
       // Change-less events don't store mappings (they are always the last event in a branch)
       if (!event.changes)
           return HistEvent.selection(selections);
       let mappedChanges = event.changes.map(mapping), before = mapping.mapDesc(event.changes, true);
       let fullMapping = event.mapped ? event.mapped.composeDesc(before) : before;
       return new HistEvent(mappedChanges, StateEffect.mapEffects(event.effects, mapping), fullMapping, event.startSelection.map(before), selections);
   }
   const joinableUserEvent = /^(input\.type|delete)($|\.)/;
   class HistoryState {
       constructor(done, undone, prevTime = 0, prevUserEvent = undefined) {
           this.done = done;
           this.undone = undone;
           this.prevTime = prevTime;
           this.prevUserEvent = prevUserEvent;
       }
       isolate() {
           return this.prevTime ? new HistoryState(this.done, this.undone) : this;
       }
       addChanges(event, time, userEvent, newGroupDelay, maxLen) {
           let done = this.done, lastEvent = done[done.length - 1];
           if (lastEvent && lastEvent.changes && !lastEvent.changes.empty && event.changes &&
               (!userEvent || joinableUserEvent.test(userEvent)) &&
               ((!lastEvent.selectionsAfter.length &&
                   time - this.prevTime < newGroupDelay &&
                   isAdjacent(lastEvent.changes, event.changes)) ||
                   // For compose (but not compose.start) events, always join with previous event
                   userEvent == "input.type.compose")) {
               done = updateBranch(done, done.length - 1, maxLen, new HistEvent(event.changes.compose(lastEvent.changes), conc(event.effects, lastEvent.effects), lastEvent.mapped, lastEvent.startSelection, none));
           }
           else {
               done = updateBranch(done, done.length, maxLen, event);
           }
           return new HistoryState(done, none, time, userEvent);
       }
       addSelection(selection, time, userEvent, newGroupDelay) {
           let last = this.done.length ? this.done[this.done.length - 1].selectionsAfter : none;
           if (last.length > 0 &&
               time - this.prevTime < newGroupDelay &&
               userEvent == this.prevUserEvent && userEvent && /^select($|\.)/.test(userEvent) &&
               eqSelectionShape(last[last.length - 1], selection))
               return this;
           return new HistoryState(addSelection(this.done, selection), this.undone, time, userEvent);
       }
       addMapping(mapping) {
           return new HistoryState(addMappingToBranch(this.done, mapping), addMappingToBranch(this.undone, mapping), this.prevTime, this.prevUserEvent);
       }
       pop(side, state, selection) {
           let branch = side == 0 /* Done */ ? this.done : this.undone;
           if (branch.length == 0)
               return null;
           let event = branch[branch.length - 1];
           if (selection && event.selectionsAfter.length) {
               return state.update({
                   selection: event.selectionsAfter[event.selectionsAfter.length - 1],
                   annotations: fromHistory.of({ side, rest: popSelection(branch) }),
                   userEvent: side == 0 /* Done */ ? "select.undo" : "select.redo",
                   scrollIntoView: true
               });
           }
           else if (!event.changes) {
               return null;
           }
           else {
               let rest = branch.length == 1 ? none : branch.slice(0, branch.length - 1);
               if (event.mapped)
                   rest = addMappingToBranch(rest, event.mapped);
               return state.update({
                   changes: event.changes,
                   selection: event.startSelection,
                   effects: event.effects,
                   annotations: fromHistory.of({ side, rest }),
                   filter: false,
                   userEvent: side == 0 /* Done */ ? "undo" : "redo",
                   scrollIntoView: true
               });
           }
       }
   }
   HistoryState.empty = /*@__PURE__*/new HistoryState(none, none);
   /**
   Default key bindings for the undo history.

   - Mod-z: [`undo`](https://codemirror.net/6/docs/ref/#commands.undo).
   - Mod-y (Mod-Shift-z on macOS) + Ctrl-Shift-z on Linux: [`redo`](https://codemirror.net/6/docs/ref/#commands.redo).
   - Mod-u: [`undoSelection`](https://codemirror.net/6/docs/ref/#commands.undoSelection).
   - Alt-u (Mod-Shift-u on macOS): [`redoSelection`](https://codemirror.net/6/docs/ref/#commands.redoSelection).
   */
   const historyKeymap = [
       { key: "Mod-z", run: undo, preventDefault: true },
       { key: "Mod-y", mac: "Mod-Shift-z", run: redo, preventDefault: true },
       { linux: "Ctrl-Shift-z", run: redo, preventDefault: true },
       { key: "Mod-u", run: undoSelection, preventDefault: true },
       { key: "Alt-u", mac: "Mod-Shift-u", run: redoSelection, preventDefault: true }
   ];

   function updateSel(sel, by) {
       return EditorSelection.create(sel.ranges.map(by), sel.mainIndex);
   }
   function setSel(state, selection) {
       return state.update({ selection, scrollIntoView: true, userEvent: "select" });
   }
   function moveSel({ state, dispatch }, how) {
       let selection = updateSel(state.selection, how);
       if (selection.eq(state.selection))
           return false;
       dispatch(setSel(state, selection));
       return true;
   }
   function rangeEnd(range, forward) {
       return EditorSelection.cursor(forward ? range.to : range.from);
   }
   function cursorByChar(view, forward) {
       return moveSel(view, range => range.empty ? view.moveByChar(range, forward) : rangeEnd(range, forward));
   }
   function ltrAtCursor(view) {
       return view.textDirectionAt(view.state.selection.main.head) == Direction.LTR;
   }
   /**
   Move the selection one character to the left (which is backward in
   left-to-right text, forward in right-to-left text).
   */
   const cursorCharLeft = view => cursorByChar(view, !ltrAtCursor(view));
   /**
   Move the selection one character to the right.
   */
   const cursorCharRight = view => cursorByChar(view, ltrAtCursor(view));
   function cursorByGroup(view, forward) {
       return moveSel(view, range => range.empty ? view.moveByGroup(range, forward) : rangeEnd(range, forward));
   }
   /**
   Move the selection to the left across one group of word or
   non-word (but also non-space) characters.
   */
   const cursorGroupLeft = view => cursorByGroup(view, !ltrAtCursor(view));
   /**
   Move the selection one group to the right.
   */
   const cursorGroupRight = view => cursorByGroup(view, ltrAtCursor(view));
   function interestingNode(state, node, bracketProp) {
       if (node.type.prop(bracketProp))
           return true;
       let len = node.to - node.from;
       return len && (len > 2 || /[^\s,.;:]/.test(state.sliceDoc(node.from, node.to))) || node.firstChild;
   }
   function moveBySyntax(state, start, forward) {
       let pos = syntaxTree(state).resolveInner(start.head);
       let bracketProp = forward ? NodeProp.closedBy : NodeProp.openedBy;
       // Scan forward through child nodes to see if there's an interesting
       // node ahead.
       for (let at = start.head;;) {
           let next = forward ? pos.childAfter(at) : pos.childBefore(at);
           if (!next)
               break;
           if (interestingNode(state, next, bracketProp))
               pos = next;
           else
               at = forward ? next.to : next.from;
       }
       let bracket = pos.type.prop(bracketProp), match, newPos;
       if (bracket && (match = forward ? matchBrackets(state, pos.from, 1) : matchBrackets(state, pos.to, -1)) && match.matched)
           newPos = forward ? match.end.to : match.end.from;
       else
           newPos = forward ? pos.to : pos.from;
       return EditorSelection.cursor(newPos, forward ? -1 : 1);
   }
   /**
   Move the cursor over the next syntactic element to the left.
   */
   const cursorSyntaxLeft = view => moveSel(view, range => moveBySyntax(view.state, range, !ltrAtCursor(view)));
   /**
   Move the cursor over the next syntactic element to the right.
   */
   const cursorSyntaxRight = view => moveSel(view, range => moveBySyntax(view.state, range, ltrAtCursor(view)));
   function cursorByLine(view, forward) {
       return moveSel(view, range => {
           if (!range.empty)
               return rangeEnd(range, forward);
           let moved = view.moveVertically(range, forward);
           return moved.head != range.head ? moved : view.moveToLineBoundary(range, forward);
       });
   }
   /**
   Move the selection one line up.
   */
   const cursorLineUp = view => cursorByLine(view, false);
   /**
   Move the selection one line down.
   */
   const cursorLineDown = view => cursorByLine(view, true);
   function pageHeight(view) {
       return Math.max(view.defaultLineHeight, Math.min(view.dom.clientHeight, innerHeight) - 5);
   }
   function cursorByPage(view, forward) {
       let { state } = view, selection = updateSel(state.selection, range => {
           return range.empty ? view.moveVertically(range, forward, pageHeight(view)) : rangeEnd(range, forward);
       });
       if (selection.eq(state.selection))
           return false;
       let startPos = view.coordsAtPos(state.selection.main.head);
       let scrollRect = view.scrollDOM.getBoundingClientRect();
       let effect;
       if (startPos && startPos.top > scrollRect.top && startPos.bottom < scrollRect.bottom &&
           startPos.top - scrollRect.top <= view.scrollDOM.scrollHeight - view.scrollDOM.scrollTop - view.scrollDOM.clientHeight)
           effect = EditorView.scrollIntoView(selection.main.head, { y: "start", yMargin: startPos.top - scrollRect.top });
       view.dispatch(setSel(state, selection), { effects: effect });
       return true;
   }
   /**
   Move the selection one page up.
   */
   const cursorPageUp = view => cursorByPage(view, false);
   /**
   Move the selection one page down.
   */
   const cursorPageDown = view => cursorByPage(view, true);
   function moveByLineBoundary(view, start, forward) {
       let line = view.lineBlockAt(start.head), moved = view.moveToLineBoundary(start, forward);
       if (moved.head == start.head && moved.head != (forward ? line.to : line.from))
           moved = view.moveToLineBoundary(start, forward, false);
       if (!forward && moved.head == line.from && line.length) {
           let space = /^\s*/.exec(view.state.sliceDoc(line.from, Math.min(line.from + 100, line.to)))[0].length;
           if (space && start.head != line.from + space)
               moved = EditorSelection.cursor(line.from + space);
       }
       return moved;
   }
   /**
   Move the selection to the next line wrap point, or to the end of
   the line if there isn't one left on this line.
   */
   const cursorLineBoundaryForward = view => moveSel(view, range => moveByLineBoundary(view, range, true));
   /**
   Move the selection to previous line wrap point, or failing that to
   the start of the line. If the line is indented, and the cursor
   isn't already at the end of the indentation, this will move to the
   end of the indentation instead of the start of the line.
   */
   const cursorLineBoundaryBackward = view => moveSel(view, range => moveByLineBoundary(view, range, false));
   /**
   Move the selection one line wrap point to the left.
   */
   const cursorLineBoundaryLeft = view => moveSel(view, range => moveByLineBoundary(view, range, !ltrAtCursor(view)));
   /**
   Move the selection one line wrap point to the right.
   */
   const cursorLineBoundaryRight = view => moveSel(view, range => moveByLineBoundary(view, range, ltrAtCursor(view)));
   /**
   Move the selection to the start of the line.
   */
   const cursorLineStart = view => moveSel(view, range => EditorSelection.cursor(view.lineBlockAt(range.head).from, 1));
   /**
   Move the selection to the end of the line.
   */
   const cursorLineEnd = view => moveSel(view, range => EditorSelection.cursor(view.lineBlockAt(range.head).to, -1));
   function toMatchingBracket(state, dispatch, extend) {
       let found = false, selection = updateSel(state.selection, range => {
           let matching = matchBrackets(state, range.head, -1)
               || matchBrackets(state, range.head, 1)
               || (range.head > 0 && matchBrackets(state, range.head - 1, 1))
               || (range.head < state.doc.length && matchBrackets(state, range.head + 1, -1));
           if (!matching || !matching.end)
               return range;
           found = true;
           let head = matching.start.from == range.head ? matching.end.to : matching.end.from;
           return extend ? EditorSelection.range(range.anchor, head) : EditorSelection.cursor(head);
       });
       if (!found)
           return false;
       dispatch(setSel(state, selection));
       return true;
   }
   /**
   Move the selection to the bracket matching the one it is currently
   on, if any.
   */
   const cursorMatchingBracket = ({ state, dispatch }) => toMatchingBracket(state, dispatch, false);
   function extendSel(view, how) {
       let selection = updateSel(view.state.selection, range => {
           let head = how(range);
           return EditorSelection.range(range.anchor, head.head, head.goalColumn);
       });
       if (selection.eq(view.state.selection))
           return false;
       view.dispatch(setSel(view.state, selection));
       return true;
   }
   function selectByChar(view, forward) {
       return extendSel(view, range => view.moveByChar(range, forward));
   }
   /**
   Move the selection head one character to the left, while leaving
   the anchor in place.
   */
   const selectCharLeft = view => selectByChar(view, !ltrAtCursor(view));
   /**
   Move the selection head one character to the right.
   */
   const selectCharRight = view => selectByChar(view, ltrAtCursor(view));
   function selectByGroup(view, forward) {
       return extendSel(view, range => view.moveByGroup(range, forward));
   }
   /**
   Move the selection head one [group](https://codemirror.net/6/docs/ref/#commands.cursorGroupLeft) to
   the left.
   */
   const selectGroupLeft = view => selectByGroup(view, !ltrAtCursor(view));
   /**
   Move the selection head one group to the right.
   */
   const selectGroupRight = view => selectByGroup(view, ltrAtCursor(view));
   /**
   Move the selection head over the next syntactic element to the left.
   */
   const selectSyntaxLeft = view => extendSel(view, range => moveBySyntax(view.state, range, !ltrAtCursor(view)));
   /**
   Move the selection head over the next syntactic element to the right.
   */
   const selectSyntaxRight = view => extendSel(view, range => moveBySyntax(view.state, range, ltrAtCursor(view)));
   function selectByLine(view, forward) {
       return extendSel(view, range => view.moveVertically(range, forward));
   }
   /**
   Move the selection head one line up.
   */
   const selectLineUp = view => selectByLine(view, false);
   /**
   Move the selection head one line down.
   */
   const selectLineDown = view => selectByLine(view, true);
   function selectByPage(view, forward) {
       return extendSel(view, range => view.moveVertically(range, forward, pageHeight(view)));
   }
   /**
   Move the selection head one page up.
   */
   const selectPageUp = view => selectByPage(view, false);
   /**
   Move the selection head one page down.
   */
   const selectPageDown = view => selectByPage(view, true);
   /**
   Move the selection head to the next line boundary.
   */
   const selectLineBoundaryForward = view => extendSel(view, range => moveByLineBoundary(view, range, true));
   /**
   Move the selection head to the previous line boundary.
   */
   const selectLineBoundaryBackward = view => extendSel(view, range => moveByLineBoundary(view, range, false));
   /**
   Move the selection head one line boundary to the left.
   */
   const selectLineBoundaryLeft = view => extendSel(view, range => moveByLineBoundary(view, range, !ltrAtCursor(view)));
   /**
   Move the selection head one line boundary to the right.
   */
   const selectLineBoundaryRight = view => extendSel(view, range => moveByLineBoundary(view, range, ltrAtCursor(view)));
   /**
   Move the selection head to the start of the line.
   */
   const selectLineStart = view => extendSel(view, range => EditorSelection.cursor(view.lineBlockAt(range.head).from));
   /**
   Move the selection head to the end of the line.
   */
   const selectLineEnd = view => extendSel(view, range => EditorSelection.cursor(view.lineBlockAt(range.head).to));
   /**
   Move the selection to the start of the document.
   */
   const cursorDocStart = ({ state, dispatch }) => {
       dispatch(setSel(state, { anchor: 0 }));
       return true;
   };
   /**
   Move the selection to the end of the document.
   */
   const cursorDocEnd = ({ state, dispatch }) => {
       dispatch(setSel(state, { anchor: state.doc.length }));
       return true;
   };
   /**
   Move the selection head to the start of the document.
   */
   const selectDocStart = ({ state, dispatch }) => {
       dispatch(setSel(state, { anchor: state.selection.main.anchor, head: 0 }));
       return true;
   };
   /**
   Move the selection head to the end of the document.
   */
   const selectDocEnd = ({ state, dispatch }) => {
       dispatch(setSel(state, { anchor: state.selection.main.anchor, head: state.doc.length }));
       return true;
   };
   /**
   Select the entire document.
   */
   const selectAll = ({ state, dispatch }) => {
       dispatch(state.update({ selection: { anchor: 0, head: state.doc.length }, userEvent: "select" }));
       return true;
   };
   /**
   Expand the selection to cover entire lines.
   */
   const selectLine = ({ state, dispatch }) => {
       let ranges = selectedLineBlocks(state).map(({ from, to }) => EditorSelection.range(from, Math.min(to + 1, state.doc.length)));
       dispatch(state.update({ selection: EditorSelection.create(ranges), userEvent: "select" }));
       return true;
   };
   /**
   Select the next syntactic construct that is larger than the
   selection. Note that this will only work insofar as the language
   [provider](https://codemirror.net/6/docs/ref/#language.language) you use builds up a full
   syntax tree.
   */
   const selectParentSyntax = ({ state, dispatch }) => {
       let selection = updateSel(state.selection, range => {
           var _a;
           let context = syntaxTree(state).resolveInner(range.head, 1);
           while (!((context.from < range.from && context.to >= range.to) ||
               (context.to > range.to && context.from <= range.from) ||
               !((_a = context.parent) === null || _a === void 0 ? void 0 : _a.parent)))
               context = context.parent;
           return EditorSelection.range(context.to, context.from);
       });
       dispatch(setSel(state, selection));
       return true;
   };
   /**
   Simplify the current selection. When multiple ranges are selected,
   reduce it to its main range. Otherwise, if the selection is
   non-empty, convert it to a cursor selection.
   */
   const simplifySelection = ({ state, dispatch }) => {
       let cur = state.selection, selection = null;
       if (cur.ranges.length > 1)
           selection = EditorSelection.create([cur.main]);
       else if (!cur.main.empty)
           selection = EditorSelection.create([EditorSelection.cursor(cur.main.head)]);
       if (!selection)
           return false;
       dispatch(setSel(state, selection));
       return true;
   };
   function deleteBy({ state, dispatch }, by) {
       if (state.readOnly)
           return false;
       let event = "delete.selection";
       let changes = state.changeByRange(range => {
           let { from, to } = range;
           if (from == to) {
               let towards = by(from);
               if (towards < from)
                   event = "delete.backward";
               else if (towards > from)
                   event = "delete.forward";
               from = Math.min(from, towards);
               to = Math.max(to, towards);
           }
           return from == to ? { range } : { changes: { from, to }, range: EditorSelection.cursor(from) };
       });
       if (changes.changes.empty)
           return false;
       dispatch(state.update(changes, {
           scrollIntoView: true,
           userEvent: event,
           effects: event == "delete.selection" ? EditorView.announce.of(state.phrase("Selection deleted")) : undefined
       }));
       return true;
   }
   function skipAtomic(target, pos, forward) {
       if (target instanceof EditorView)
           for (let ranges of target.state.facet(EditorView.atomicRanges).map(f => f(target)))
               ranges.between(pos, pos, (from, to) => {
                   if (from < pos && to > pos)
                       pos = forward ? to : from;
               });
       return pos;
   }
   const deleteByChar = (target, forward) => deleteBy(target, pos => {
       let { state } = target, line = state.doc.lineAt(pos), before, targetPos;
       if (!forward && pos > line.from && pos < line.from + 200 &&
           !/[^ \t]/.test(before = line.text.slice(0, pos - line.from))) {
           if (before[before.length - 1] == "\t")
               return pos - 1;
           let col = countColumn(before, state.tabSize), drop = col % getIndentUnit(state) || getIndentUnit(state);
           for (let i = 0; i < drop && before[before.length - 1 - i] == " "; i++)
               pos--;
           targetPos = pos;
       }
       else {
           targetPos = findClusterBreak(line.text, pos - line.from, forward, forward) + line.from;
           if (targetPos == pos && line.number != (forward ? state.doc.lines : 1))
               targetPos += forward ? 1 : -1;
       }
       return skipAtomic(target, targetPos, forward);
   });
   /**
   Delete the selection, or, for cursor selections, the character
   before the cursor.
   */
   const deleteCharBackward = view => deleteByChar(view, false);
   /**
   Delete the selection or the character after the cursor.
   */
   const deleteCharForward = view => deleteByChar(view, true);
   const deleteByGroup = (target, forward) => deleteBy(target, start => {
       let pos = start, { state } = target, line = state.doc.lineAt(pos);
       let categorize = state.charCategorizer(pos);
       for (let cat = null;;) {
           if (pos == (forward ? line.to : line.from)) {
               if (pos == start && line.number != (forward ? state.doc.lines : 1))
                   pos += forward ? 1 : -1;
               break;
           }
           let next = findClusterBreak(line.text, pos - line.from, forward) + line.from;
           let nextChar = line.text.slice(Math.min(pos, next) - line.from, Math.max(pos, next) - line.from);
           let nextCat = categorize(nextChar);
           if (cat != null && nextCat != cat)
               break;
           if (nextChar != " " || pos != start)
               cat = nextCat;
           pos = next;
       }
       return skipAtomic(target, pos, forward);
   });
   /**
   Delete the selection or backward until the end of the next
   [group](https://codemirror.net/6/docs/ref/#view.EditorView.moveByGroup), only skipping groups of
   whitespace when they consist of a single space.
   */
   const deleteGroupBackward = target => deleteByGroup(target, false);
   /**
   Delete the selection or forward until the end of the next group.
   */
   const deleteGroupForward = target => deleteByGroup(target, true);
   /**
   Delete the selection, or, if it is a cursor selection, delete to
   the end of the line. If the cursor is directly at the end of the
   line, delete the line break after it.
   */
   const deleteToLineEnd = view => deleteBy(view, pos => {
       let lineEnd = view.lineBlockAt(pos).to;
       return skipAtomic(view, pos < lineEnd ? lineEnd : Math.min(view.state.doc.length, pos + 1), true);
   });
   /**
   Delete the selection, or, if it is a cursor selection, delete to
   the start of the line. If the cursor is directly at the start of the
   line, delete the line break before it.
   */
   const deleteToLineStart = view => deleteBy(view, pos => {
       let lineStart = view.lineBlockAt(pos).from;
       return skipAtomic(view, pos > lineStart ? lineStart : Math.max(0, pos - 1), false);
   });
   /**
   Replace each selection range with a line break, leaving the cursor
   on the line before the break.
   */
   const splitLine = ({ state, dispatch }) => {
       if (state.readOnly)
           return false;
       let changes = state.changeByRange(range => {
           return { changes: { from: range.from, to: range.to, insert: Text.of(["", ""]) },
               range: EditorSelection.cursor(range.from) };
       });
       dispatch(state.update(changes, { scrollIntoView: true, userEvent: "input" }));
       return true;
   };
   /**
   Flip the characters before and after the cursor(s).
   */
   const transposeChars = ({ state, dispatch }) => {
       if (state.readOnly)
           return false;
       let changes = state.changeByRange(range => {
           if (!range.empty || range.from == 0 || range.from == state.doc.length)
               return { range };
           let pos = range.from, line = state.doc.lineAt(pos);
           let from = pos == line.from ? pos - 1 : findClusterBreak(line.text, pos - line.from, false) + line.from;
           let to = pos == line.to ? pos + 1 : findClusterBreak(line.text, pos - line.from, true) + line.from;
           return { changes: { from, to, insert: state.doc.slice(pos, to).append(state.doc.slice(from, pos)) },
               range: EditorSelection.cursor(to) };
       });
       if (changes.changes.empty)
           return false;
       dispatch(state.update(changes, { scrollIntoView: true, userEvent: "move.character" }));
       return true;
   };
   function selectedLineBlocks(state) {
       let blocks = [], upto = -1;
       for (let range of state.selection.ranges) {
           let startLine = state.doc.lineAt(range.from), endLine = state.doc.lineAt(range.to);
           if (!range.empty && range.to == endLine.from)
               endLine = state.doc.lineAt(range.to - 1);
           if (upto >= startLine.number) {
               let prev = blocks[blocks.length - 1];
               prev.to = endLine.to;
               prev.ranges.push(range);
           }
           else {
               blocks.push({ from: startLine.from, to: endLine.to, ranges: [range] });
           }
           upto = endLine.number + 1;
       }
       return blocks;
   }
   function moveLine(state, dispatch, forward) {
       if (state.readOnly)
           return false;
       let changes = [], ranges = [];
       for (let block of selectedLineBlocks(state)) {
           if (forward ? block.to == state.doc.length : block.from == 0)
               continue;
           let nextLine = state.doc.lineAt(forward ? block.to + 1 : block.from - 1);
           let size = nextLine.length + 1;
           if (forward) {
               changes.push({ from: block.to, to: nextLine.to }, { from: block.from, insert: nextLine.text + state.lineBreak });
               for (let r of block.ranges)
                   ranges.push(EditorSelection.range(Math.min(state.doc.length, r.anchor + size), Math.min(state.doc.length, r.head + size)));
           }
           else {
               changes.push({ from: nextLine.from, to: block.from }, { from: block.to, insert: state.lineBreak + nextLine.text });
               for (let r of block.ranges)
                   ranges.push(EditorSelection.range(r.anchor - size, r.head - size));
           }
       }
       if (!changes.length)
           return false;
       dispatch(state.update({
           changes,
           scrollIntoView: true,
           selection: EditorSelection.create(ranges, state.selection.mainIndex),
           userEvent: "move.line"
       }));
       return true;
   }
   /**
   Move the selected lines up one line.
   */
   const moveLineUp = ({ state, dispatch }) => moveLine(state, dispatch, false);
   /**
   Move the selected lines down one line.
   */
   const moveLineDown = ({ state, dispatch }) => moveLine(state, dispatch, true);
   function copyLine(state, dispatch, forward) {
       if (state.readOnly)
           return false;
       let changes = [];
       for (let block of selectedLineBlocks(state)) {
           if (forward)
               changes.push({ from: block.from, insert: state.doc.slice(block.from, block.to) + state.lineBreak });
           else
               changes.push({ from: block.to, insert: state.lineBreak + state.doc.slice(block.from, block.to) });
       }
       dispatch(state.update({ changes, scrollIntoView: true, userEvent: "input.copyline" }));
       return true;
   }
   /**
   Create a copy of the selected lines. Keep the selection in the top copy.
   */
   const copyLineUp = ({ state, dispatch }) => copyLine(state, dispatch, false);
   /**
   Create a copy of the selected lines. Keep the selection in the bottom copy.
   */
   const copyLineDown = ({ state, dispatch }) => copyLine(state, dispatch, true);
   /**
   Delete selected lines.
   */
   const deleteLine = view => {
       if (view.state.readOnly)
           return false;
       let { state } = view, changes = state.changes(selectedLineBlocks(state).map(({ from, to }) => {
           if (from > 0)
               from--;
           else if (to < state.doc.length)
               to++;
           return { from, to };
       }));
       let selection = updateSel(state.selection, range => view.moveVertically(range, true)).map(changes);
       view.dispatch({ changes, selection, scrollIntoView: true, userEvent: "delete.line" });
       return true;
   };
   function isBetweenBrackets(state, pos) {
       if (/\(\)|\[\]|\{\}/.test(state.sliceDoc(pos - 1, pos + 1)))
           return { from: pos, to: pos };
       let context = syntaxTree(state).resolveInner(pos);
       let before = context.childBefore(pos), after = context.childAfter(pos), closedBy;
       if (before && after && before.to <= pos && after.from >= pos &&
           (closedBy = before.type.prop(NodeProp.closedBy)) && closedBy.indexOf(after.name) > -1 &&
           state.doc.lineAt(before.to).from == state.doc.lineAt(after.from).from)
           return { from: before.to, to: after.from };
       return null;
   }
   /**
   Replace the selection with a newline and indent the newly created
   line(s). If the current line consists only of whitespace, this
   will also delete that whitespace. When the cursor is between
   matching brackets, an additional newline will be inserted after
   the cursor.
   */
   const insertNewlineAndIndent = /*@__PURE__*/newlineAndIndent(false);
   /**
   Create a blank, indented line below the current line.
   */
   const insertBlankLine = /*@__PURE__*/newlineAndIndent(true);
   function newlineAndIndent(atEof) {
       return ({ state, dispatch }) => {
           if (state.readOnly)
               return false;
           let changes = state.changeByRange(range => {
               let { from, to } = range, line = state.doc.lineAt(from);
               let explode = !atEof && from == to && isBetweenBrackets(state, from);
               if (atEof)
                   from = to = (to <= line.to ? line : state.doc.lineAt(to)).to;
               let cx = new IndentContext(state, { simulateBreak: from, simulateDoubleBreak: !!explode });
               let indent = getIndentation(cx, from);
               if (indent == null)
                   indent = /^\s*/.exec(state.doc.lineAt(from).text)[0].length;
               while (to < line.to && /\s/.test(line.text[to - line.from]))
                   to++;
               if (explode)
                   ({ from, to } = explode);
               else if (from > line.from && from < line.from + 100 && !/\S/.test(line.text.slice(0, from)))
                   from = line.from;
               let insert = ["", indentString(state, indent)];
               if (explode)
                   insert.push(indentString(state, cx.lineIndent(line.from, -1)));
               return { changes: { from, to, insert: Text.of(insert) },
                   range: EditorSelection.cursor(from + 1 + insert[1].length) };
           });
           dispatch(state.update(changes, { scrollIntoView: true, userEvent: "input" }));
           return true;
       };
   }
   function changeBySelectedLine(state, f) {
       let atLine = -1;
       return state.changeByRange(range => {
           let changes = [];
           for (let pos = range.from; pos <= range.to;) {
               let line = state.doc.lineAt(pos);
               if (line.number > atLine && (range.empty || range.to > line.from)) {
                   f(line, changes, range);
                   atLine = line.number;
               }
               pos = line.to + 1;
           }
           let changeSet = state.changes(changes);
           return { changes,
               range: EditorSelection.range(changeSet.mapPos(range.anchor, 1), changeSet.mapPos(range.head, 1)) };
       });
   }
   /**
   Auto-indent the selected lines. This uses the [indentation service
   facet](https://codemirror.net/6/docs/ref/#language.indentService) as source for auto-indent
   information.
   */
   const indentSelection = ({ state, dispatch }) => {
       if (state.readOnly)
           return false;
       let updated = Object.create(null);
       let context = new IndentContext(state, { overrideIndentation: start => {
               let found = updated[start];
               return found == null ? -1 : found;
           } });
       let changes = changeBySelectedLine(state, (line, changes, range) => {
           let indent = getIndentation(context, line.from);
           if (indent == null)
               return;
           if (!/\S/.test(line.text))
               indent = 0;
           let cur = /^\s*/.exec(line.text)[0];
           let norm = indentString(state, indent);
           if (cur != norm || range.from < line.from + cur.length) {
               updated[line.from] = indent;
               changes.push({ from: line.from, to: line.from + cur.length, insert: norm });
           }
       });
       if (!changes.changes.empty)
           dispatch(state.update(changes, { userEvent: "indent" }));
       return true;
   };
   /**
   Add a [unit](https://codemirror.net/6/docs/ref/#language.indentUnit) of indentation to all selected
   lines.
   */
   const indentMore = ({ state, dispatch }) => {
       if (state.readOnly)
           return false;
       dispatch(state.update(changeBySelectedLine(state, (line, changes) => {
           changes.push({ from: line.from, insert: state.facet(indentUnit) });
       }), { userEvent: "input.indent" }));
       return true;
   };
   /**
   Remove a [unit](https://codemirror.net/6/docs/ref/#language.indentUnit) of indentation from all
   selected lines.
   */
   const indentLess = ({ state, dispatch }) => {
       if (state.readOnly)
           return false;
       dispatch(state.update(changeBySelectedLine(state, (line, changes) => {
           let space = /^\s*/.exec(line.text)[0];
           if (!space)
               return;
           let col = countColumn(space, state.tabSize), keep = 0;
           let insert = indentString(state, Math.max(0, col - getIndentUnit(state)));
           while (keep < space.length && keep < insert.length && space.charCodeAt(keep) == insert.charCodeAt(keep))
               keep++;
           changes.push({ from: line.from + keep, to: line.from + space.length, insert: insert.slice(keep) });
       }), { userEvent: "delete.dedent" }));
       return true;
   };
   /**
   Array of key bindings containing the Emacs-style bindings that are
   available on macOS by default.

    - Ctrl-b: [`cursorCharLeft`](https://codemirror.net/6/docs/ref/#commands.cursorCharLeft) ([`selectCharLeft`](https://codemirror.net/6/docs/ref/#commands.selectCharLeft) with Shift)
    - Ctrl-f: [`cursorCharRight`](https://codemirror.net/6/docs/ref/#commands.cursorCharRight) ([`selectCharRight`](https://codemirror.net/6/docs/ref/#commands.selectCharRight) with Shift)
    - Ctrl-p: [`cursorLineUp`](https://codemirror.net/6/docs/ref/#commands.cursorLineUp) ([`selectLineUp`](https://codemirror.net/6/docs/ref/#commands.selectLineUp) with Shift)
    - Ctrl-n: [`cursorLineDown`](https://codemirror.net/6/docs/ref/#commands.cursorLineDown) ([`selectLineDown`](https://codemirror.net/6/docs/ref/#commands.selectLineDown) with Shift)
    - Ctrl-a: [`cursorLineStart`](https://codemirror.net/6/docs/ref/#commands.cursorLineStart) ([`selectLineStart`](https://codemirror.net/6/docs/ref/#commands.selectLineStart) with Shift)
    - Ctrl-e: [`cursorLineEnd`](https://codemirror.net/6/docs/ref/#commands.cursorLineEnd) ([`selectLineEnd`](https://codemirror.net/6/docs/ref/#commands.selectLineEnd) with Shift)
    - Ctrl-d: [`deleteCharForward`](https://codemirror.net/6/docs/ref/#commands.deleteCharForward)
    - Ctrl-h: [`deleteCharBackward`](https://codemirror.net/6/docs/ref/#commands.deleteCharBackward)
    - Ctrl-k: [`deleteToLineEnd`](https://codemirror.net/6/docs/ref/#commands.deleteToLineEnd)
    - Ctrl-Alt-h: [`deleteGroupBackward`](https://codemirror.net/6/docs/ref/#commands.deleteGroupBackward)
    - Ctrl-o: [`splitLine`](https://codemirror.net/6/docs/ref/#commands.splitLine)
    - Ctrl-t: [`transposeChars`](https://codemirror.net/6/docs/ref/#commands.transposeChars)
    - Ctrl-v: [`cursorPageDown`](https://codemirror.net/6/docs/ref/#commands.cursorPageDown)
    - Alt-v: [`cursorPageUp`](https://codemirror.net/6/docs/ref/#commands.cursorPageUp)
   */
   const emacsStyleKeymap = [
       { key: "Ctrl-b", run: cursorCharLeft, shift: selectCharLeft, preventDefault: true },
       { key: "Ctrl-f", run: cursorCharRight, shift: selectCharRight },
       { key: "Ctrl-p", run: cursorLineUp, shift: selectLineUp },
       { key: "Ctrl-n", run: cursorLineDown, shift: selectLineDown },
       { key: "Ctrl-a", run: cursorLineStart, shift: selectLineStart },
       { key: "Ctrl-e", run: cursorLineEnd, shift: selectLineEnd },
       { key: "Ctrl-d", run: deleteCharForward },
       { key: "Ctrl-h", run: deleteCharBackward },
       { key: "Ctrl-k", run: deleteToLineEnd },
       { key: "Ctrl-Alt-h", run: deleteGroupBackward },
       { key: "Ctrl-o", run: splitLine },
       { key: "Ctrl-t", run: transposeChars },
       { key: "Ctrl-v", run: cursorPageDown },
   ];
   /**
   An array of key bindings closely sticking to platform-standard or
   widely used bindings. (This includes the bindings from
   [`emacsStyleKeymap`](https://codemirror.net/6/docs/ref/#commands.emacsStyleKeymap), with their `key`
   property changed to `mac`.)

    - ArrowLeft: [`cursorCharLeft`](https://codemirror.net/6/docs/ref/#commands.cursorCharLeft) ([`selectCharLeft`](https://codemirror.net/6/docs/ref/#commands.selectCharLeft) with Shift)
    - ArrowRight: [`cursorCharRight`](https://codemirror.net/6/docs/ref/#commands.cursorCharRight) ([`selectCharRight`](https://codemirror.net/6/docs/ref/#commands.selectCharRight) with Shift)
    - Ctrl-ArrowLeft (Alt-ArrowLeft on macOS): [`cursorGroupLeft`](https://codemirror.net/6/docs/ref/#commands.cursorGroupLeft) ([`selectGroupLeft`](https://codemirror.net/6/docs/ref/#commands.selectGroupLeft) with Shift)
    - Ctrl-ArrowRight (Alt-ArrowRight on macOS): [`cursorGroupRight`](https://codemirror.net/6/docs/ref/#commands.cursorGroupRight) ([`selectGroupRight`](https://codemirror.net/6/docs/ref/#commands.selectGroupRight) with Shift)
    - Cmd-ArrowLeft (on macOS): [`cursorLineStart`](https://codemirror.net/6/docs/ref/#commands.cursorLineStart) ([`selectLineStart`](https://codemirror.net/6/docs/ref/#commands.selectLineStart) with Shift)
    - Cmd-ArrowRight (on macOS): [`cursorLineEnd`](https://codemirror.net/6/docs/ref/#commands.cursorLineEnd) ([`selectLineEnd`](https://codemirror.net/6/docs/ref/#commands.selectLineEnd) with Shift)
    - ArrowUp: [`cursorLineUp`](https://codemirror.net/6/docs/ref/#commands.cursorLineUp) ([`selectLineUp`](https://codemirror.net/6/docs/ref/#commands.selectLineUp) with Shift)
    - ArrowDown: [`cursorLineDown`](https://codemirror.net/6/docs/ref/#commands.cursorLineDown) ([`selectLineDown`](https://codemirror.net/6/docs/ref/#commands.selectLineDown) with Shift)
    - Cmd-ArrowUp (on macOS): [`cursorDocStart`](https://codemirror.net/6/docs/ref/#commands.cursorDocStart) ([`selectDocStart`](https://codemirror.net/6/docs/ref/#commands.selectDocStart) with Shift)
    - Cmd-ArrowDown (on macOS): [`cursorDocEnd`](https://codemirror.net/6/docs/ref/#commands.cursorDocEnd) ([`selectDocEnd`](https://codemirror.net/6/docs/ref/#commands.selectDocEnd) with Shift)
    - Ctrl-ArrowUp (on macOS): [`cursorPageUp`](https://codemirror.net/6/docs/ref/#commands.cursorPageUp) ([`selectPageUp`](https://codemirror.net/6/docs/ref/#commands.selectPageUp) with Shift)
    - Ctrl-ArrowDown (on macOS): [`cursorPageDown`](https://codemirror.net/6/docs/ref/#commands.cursorPageDown) ([`selectPageDown`](https://codemirror.net/6/docs/ref/#commands.selectPageDown) with Shift)
    - PageUp: [`cursorPageUp`](https://codemirror.net/6/docs/ref/#commands.cursorPageUp) ([`selectPageUp`](https://codemirror.net/6/docs/ref/#commands.selectPageUp) with Shift)
    - PageDown: [`cursorPageDown`](https://codemirror.net/6/docs/ref/#commands.cursorPageDown) ([`selectPageDown`](https://codemirror.net/6/docs/ref/#commands.selectPageDown) with Shift)
    - Home: [`cursorLineBoundaryBackward`](https://codemirror.net/6/docs/ref/#commands.cursorLineBoundaryBackward) ([`selectLineBoundaryBackward`](https://codemirror.net/6/docs/ref/#commands.selectLineBoundaryBackward) with Shift)
    - End: [`cursorLineBoundaryForward`](https://codemirror.net/6/docs/ref/#commands.cursorLineBoundaryForward) ([`selectLineBoundaryForward`](https://codemirror.net/6/docs/ref/#commands.selectLineBoundaryForward) with Shift)
    - Ctrl-Home (Cmd-Home on macOS): [`cursorDocStart`](https://codemirror.net/6/docs/ref/#commands.cursorDocStart) ([`selectDocStart`](https://codemirror.net/6/docs/ref/#commands.selectDocStart) with Shift)
    - Ctrl-End (Cmd-Home on macOS): [`cursorDocEnd`](https://codemirror.net/6/docs/ref/#commands.cursorDocEnd) ([`selectDocEnd`](https://codemirror.net/6/docs/ref/#commands.selectDocEnd) with Shift)
    - Enter: [`insertNewlineAndIndent`](https://codemirror.net/6/docs/ref/#commands.insertNewlineAndIndent)
    - Ctrl-a (Cmd-a on macOS): [`selectAll`](https://codemirror.net/6/docs/ref/#commands.selectAll)
    - Backspace: [`deleteCharBackward`](https://codemirror.net/6/docs/ref/#commands.deleteCharBackward)
    - Delete: [`deleteCharForward`](https://codemirror.net/6/docs/ref/#commands.deleteCharForward)
    - Ctrl-Backspace (Alt-Backspace on macOS): [`deleteGroupBackward`](https://codemirror.net/6/docs/ref/#commands.deleteGroupBackward)
    - Ctrl-Delete (Alt-Delete on macOS): [`deleteGroupForward`](https://codemirror.net/6/docs/ref/#commands.deleteGroupForward)
    - Cmd-Backspace (macOS): [`deleteToLineStart`](https://codemirror.net/6/docs/ref/#commands.deleteToLineStart).
    - Cmd-Delete (macOS): [`deleteToLineEnd`](https://codemirror.net/6/docs/ref/#commands.deleteToLineEnd).
   */
   const standardKeymap = /*@__PURE__*/[
       { key: "ArrowLeft", run: cursorCharLeft, shift: selectCharLeft, preventDefault: true },
       { key: "Mod-ArrowLeft", mac: "Alt-ArrowLeft", run: cursorGroupLeft, shift: selectGroupLeft, preventDefault: true },
       { mac: "Cmd-ArrowLeft", run: cursorLineBoundaryLeft, shift: selectLineBoundaryLeft, preventDefault: true },
       { key: "ArrowRight", run: cursorCharRight, shift: selectCharRight, preventDefault: true },
       { key: "Mod-ArrowRight", mac: "Alt-ArrowRight", run: cursorGroupRight, shift: selectGroupRight, preventDefault: true },
       { mac: "Cmd-ArrowRight", run: cursorLineBoundaryRight, shift: selectLineBoundaryRight, preventDefault: true },
       { key: "ArrowUp", run: cursorLineUp, shift: selectLineUp, preventDefault: true },
       { mac: "Cmd-ArrowUp", run: cursorDocStart, shift: selectDocStart },
       { mac: "Ctrl-ArrowUp", run: cursorPageUp, shift: selectPageUp },
       { key: "ArrowDown", run: cursorLineDown, shift: selectLineDown, preventDefault: true },
       { mac: "Cmd-ArrowDown", run: cursorDocEnd, shift: selectDocEnd },
       { mac: "Ctrl-ArrowDown", run: cursorPageDown, shift: selectPageDown },
       { key: "PageUp", run: cursorPageUp, shift: selectPageUp },
       { key: "PageDown", run: cursorPageDown, shift: selectPageDown },
       { key: "Home", run: cursorLineBoundaryBackward, shift: selectLineBoundaryBackward, preventDefault: true },
       { key: "Mod-Home", run: cursorDocStart, shift: selectDocStart },
       { key: "End", run: cursorLineBoundaryForward, shift: selectLineBoundaryForward, preventDefault: true },
       { key: "Mod-End", run: cursorDocEnd, shift: selectDocEnd },
       { key: "Enter", run: insertNewlineAndIndent },
       { key: "Mod-a", run: selectAll },
       { key: "Backspace", run: deleteCharBackward, shift: deleteCharBackward },
       { key: "Delete", run: deleteCharForward },
       { key: "Mod-Backspace", mac: "Alt-Backspace", run: deleteGroupBackward },
       { key: "Mod-Delete", mac: "Alt-Delete", run: deleteGroupForward },
       { mac: "Mod-Backspace", run: deleteToLineStart },
       { mac: "Mod-Delete", run: deleteToLineEnd }
   ].concat(/*@__PURE__*/emacsStyleKeymap.map(b => ({ mac: b.key, run: b.run, shift: b.shift })));
   /**
   The default keymap. Includes all bindings from
   [`standardKeymap`](https://codemirror.net/6/docs/ref/#commands.standardKeymap) plus the following:

   - Alt-ArrowLeft (Ctrl-ArrowLeft on macOS): [`cursorSyntaxLeft`](https://codemirror.net/6/docs/ref/#commands.cursorSyntaxLeft) ([`selectSyntaxLeft`](https://codemirror.net/6/docs/ref/#commands.selectSyntaxLeft) with Shift)
   - Alt-ArrowRight (Ctrl-ArrowRight on macOS): [`cursorSyntaxRight`](https://codemirror.net/6/docs/ref/#commands.cursorSyntaxRight) ([`selectSyntaxRight`](https://codemirror.net/6/docs/ref/#commands.selectSyntaxRight) with Shift)
   - Alt-ArrowUp: [`moveLineUp`](https://codemirror.net/6/docs/ref/#commands.moveLineUp)
   - Alt-ArrowDown: [`moveLineDown`](https://codemirror.net/6/docs/ref/#commands.moveLineDown)
   - Shift-Alt-ArrowUp: [`copyLineUp`](https://codemirror.net/6/docs/ref/#commands.copyLineUp)
   - Shift-Alt-ArrowDown: [`copyLineDown`](https://codemirror.net/6/docs/ref/#commands.copyLineDown)
   - Escape: [`simplifySelection`](https://codemirror.net/6/docs/ref/#commands.simplifySelection)
   - Ctrl-Enter (Comd-Enter on macOS): [`insertBlankLine`](https://codemirror.net/6/docs/ref/#commands.insertBlankLine)
   - Alt-l (Ctrl-l on macOS): [`selectLine`](https://codemirror.net/6/docs/ref/#commands.selectLine)
   - Ctrl-i (Cmd-i on macOS): [`selectParentSyntax`](https://codemirror.net/6/docs/ref/#commands.selectParentSyntax)
   - Ctrl-[ (Cmd-[ on macOS): [`indentLess`](https://codemirror.net/6/docs/ref/#commands.indentLess)
   - Ctrl-] (Cmd-] on macOS): [`indentMore`](https://codemirror.net/6/docs/ref/#commands.indentMore)
   - Ctrl-Alt-\\ (Cmd-Alt-\\ on macOS): [`indentSelection`](https://codemirror.net/6/docs/ref/#commands.indentSelection)
   - Shift-Ctrl-k (Shift-Cmd-k on macOS): [`deleteLine`](https://codemirror.net/6/docs/ref/#commands.deleteLine)
   - Shift-Ctrl-\\ (Shift-Cmd-\\ on macOS): [`cursorMatchingBracket`](https://codemirror.net/6/docs/ref/#commands.cursorMatchingBracket)
   - Ctrl-/ (Cmd-/ on macOS): [`toggleComment`](https://codemirror.net/6/docs/ref/#commands.toggleComment).
   - Shift-Alt-a: [`toggleBlockComment`](https://codemirror.net/6/docs/ref/#commands.toggleBlockComment).
   */
   const defaultKeymap = /*@__PURE__*/[
       { key: "Alt-ArrowLeft", mac: "Ctrl-ArrowLeft", run: cursorSyntaxLeft, shift: selectSyntaxLeft },
       { key: "Alt-ArrowRight", mac: "Ctrl-ArrowRight", run: cursorSyntaxRight, shift: selectSyntaxRight },
       { key: "Alt-ArrowUp", run: moveLineUp },
       { key: "Shift-Alt-ArrowUp", run: copyLineUp },
       { key: "Alt-ArrowDown", run: moveLineDown },
       { key: "Shift-Alt-ArrowDown", run: copyLineDown },
       { key: "Escape", run: simplifySelection },
       { key: "Mod-Enter", run: insertBlankLine },
       { key: "Alt-l", mac: "Ctrl-l", run: selectLine },
       { key: "Mod-i", run: selectParentSyntax, preventDefault: true },
       { key: "Mod-[", run: indentLess },
       { key: "Mod-]", run: indentMore },
       { key: "Mod-Alt-\\", run: indentSelection },
       { key: "Shift-Mod-k", run: deleteLine },
       { key: "Shift-Mod-\\", run: cursorMatchingBracket },
       { key: "Mod-/", run: toggleComment },
       { key: "Alt-A", run: toggleBlockComment }
   ].concat(standardKeymap);

   /**
   A minimal set of extensions to create a functional editor. Only
   includes [the default keymap](https://codemirror.net/6/docs/ref/#commands.defaultKeymap), [undo
   history](https://codemirror.net/6/docs/ref/#commands.history), [special character
   highlighting](https://codemirror.net/6/docs/ref/#view.highlightSpecialChars), [custom selection
   drawing](https://codemirror.net/6/docs/ref/#view.drawSelection), and [default highlight
   style](https://codemirror.net/6/docs/ref/#language.defaultHighlightStyle).
   */
   const minimalSetup = /*@__PURE__*/(() => [
       highlightSpecialChars(),
       history(),
       drawSelection(),
       syntaxHighlighting(defaultHighlightStyle, { fallback: true }),
       keymap.of([
           ...defaultKeymap,
           ...historyKeymap,
       ])
   ])();

   /// A parse stack. These are used internally by the parser to track
   /// parsing progress. They also provide some properties and methods
   /// that external code such as a tokenizer can use to get information
   /// about the parse state.
   class Stack {
       /// @internal
       constructor(
       /// The parse that this stack is part of @internal
       p, 
       /// Holds state, input pos, buffer index triplets for all but the
       /// top state @internal
       stack, 
       /// The current parse state @internal
       state, 
       // The position at which the next reduce should take place. This
       // can be less than `this.pos` when skipped expressions have been
       // added to the stack (which should be moved outside of the next
       // reduction)
       /// @internal
       reducePos, 
       /// The input position up to which this stack has parsed.
       pos, 
       /// The dynamic score of the stack, including dynamic precedence
       /// and error-recovery penalties
       /// @internal
       score, 
       // The output buffer. Holds (type, start, end, size) quads
       // representing nodes created by the parser, where `size` is
       // amount of buffer array entries covered by this node.
       /// @internal
       buffer, 
       // The base offset of the buffer. When stacks are split, the split
       // instance shared the buffer history with its parent up to
       // `bufferBase`, which is the absolute offset (including the
       // offset of previous splits) into the buffer at which this stack
       // starts writing.
       /// @internal
       bufferBase, 
       /// @internal
       curContext, 
       /// @internal
       lookAhead = 0, 
       // A parent stack from which this was split off, if any. This is
       // set up so that it always points to a stack that has some
       // additional buffer content, never to a stack with an equal
       // `bufferBase`.
       /// @internal
       parent) {
           this.p = p;
           this.stack = stack;
           this.state = state;
           this.reducePos = reducePos;
           this.pos = pos;
           this.score = score;
           this.buffer = buffer;
           this.bufferBase = bufferBase;
           this.curContext = curContext;
           this.lookAhead = lookAhead;
           this.parent = parent;
       }
       /// @internal
       toString() {
           return `[${this.stack.filter((_, i) => i % 3 == 0).concat(this.state)}]@${this.pos}${this.score ? "!" + this.score : ""}`;
       }
       // Start an empty stack
       /// @internal
       static start(p, state, pos = 0) {
           let cx = p.parser.context;
           return new Stack(p, [], state, pos, pos, 0, [], 0, cx ? new StackContext(cx, cx.start) : null, 0, null);
       }
       /// The stack's current [context](#lr.ContextTracker) value, if
       /// any. Its type will depend on the context tracker's type
       /// parameter, or it will be `null` if there is no context
       /// tracker.
       get context() { return this.curContext ? this.curContext.context : null; }
       // Push a state onto the stack, tracking its start position as well
       // as the buffer base at that point.
       /// @internal
       pushState(state, start) {
           this.stack.push(this.state, start, this.bufferBase + this.buffer.length);
           this.state = state;
       }
       // Apply a reduce action
       /// @internal
       reduce(action) {
           let depth = action >> 19 /* ReduceDepthShift */, type = action & 65535 /* ValueMask */;
           let { parser } = this.p;
           let dPrec = parser.dynamicPrecedence(type);
           if (dPrec)
               this.score += dPrec;
           if (depth == 0) {
               this.pushState(parser.getGoto(this.state, type, true), this.reducePos);
               // Zero-depth reductions are a special case—they add stuff to
               // the stack without popping anything off.
               if (type < parser.minRepeatTerm)
                   this.storeNode(type, this.reducePos, this.reducePos, 4, true);
               this.reduceContext(type, this.reducePos);
               return;
           }
           // Find the base index into `this.stack`, content after which will
           // be dropped. Note that with `StayFlag` reductions we need to
           // consume two extra frames (the dummy parent node for the skipped
           // expression and the state that we'll be staying in, which should
           // be moved to `this.state`).
           let base = this.stack.length - ((depth - 1) * 3) - (action & 262144 /* StayFlag */ ? 6 : 0);
           let start = this.stack[base - 2];
           let bufferBase = this.stack[base - 1], count = this.bufferBase + this.buffer.length - bufferBase;
           // Store normal terms or `R -> R R` repeat reductions
           if (type < parser.minRepeatTerm || (action & 131072 /* RepeatFlag */)) {
               let pos = parser.stateFlag(this.state, 1 /* Skipped */) ? this.pos : this.reducePos;
               this.storeNode(type, start, pos, count + 4, true);
           }
           if (action & 262144 /* StayFlag */) {
               this.state = this.stack[base];
           }
           else {
               let baseStateID = this.stack[base - 3];
               this.state = parser.getGoto(baseStateID, type, true);
           }
           while (this.stack.length > base)
               this.stack.pop();
           this.reduceContext(type, start);
       }
       // Shift a value into the buffer
       /// @internal
       storeNode(term, start, end, size = 4, isReduce = false) {
           if (term == 0 /* Err */ &&
               (!this.stack.length || this.stack[this.stack.length - 1] < this.buffer.length + this.bufferBase)) {
               // Try to omit/merge adjacent error nodes
               let cur = this, top = this.buffer.length;
               if (top == 0 && cur.parent) {
                   top = cur.bufferBase - cur.parent.bufferBase;
                   cur = cur.parent;
               }
               if (top > 0 && cur.buffer[top - 4] == 0 /* Err */ && cur.buffer[top - 1] > -1) {
                   if (start == end)
                       return;
                   if (cur.buffer[top - 2] >= start) {
                       cur.buffer[top - 2] = end;
                       return;
                   }
               }
           }
           if (!isReduce || this.pos == end) { // Simple case, just append
               this.buffer.push(term, start, end, size);
           }
           else { // There may be skipped nodes that have to be moved forward
               let index = this.buffer.length;
               if (index > 0 && this.buffer[index - 4] != 0 /* Err */)
                   while (index > 0 && this.buffer[index - 2] > end) {
                       // Move this record forward
                       this.buffer[index] = this.buffer[index - 4];
                       this.buffer[index + 1] = this.buffer[index - 3];
                       this.buffer[index + 2] = this.buffer[index - 2];
                       this.buffer[index + 3] = this.buffer[index - 1];
                       index -= 4;
                       if (size > 4)
                           size -= 4;
                   }
               this.buffer[index] = term;
               this.buffer[index + 1] = start;
               this.buffer[index + 2] = end;
               this.buffer[index + 3] = size;
           }
       }
       // Apply a shift action
       /// @internal
       shift(action, next, nextEnd) {
           let start = this.pos;
           if (action & 131072 /* GotoFlag */) {
               this.pushState(action & 65535 /* ValueMask */, this.pos);
           }
           else if ((action & 262144 /* StayFlag */) == 0) { // Regular shift
               let nextState = action, { parser } = this.p;
               if (nextEnd > this.pos || next <= parser.maxNode) {
                   this.pos = nextEnd;
                   if (!parser.stateFlag(nextState, 1 /* Skipped */))
                       this.reducePos = nextEnd;
               }
               this.pushState(nextState, start);
               this.shiftContext(next, start);
               if (next <= parser.maxNode)
                   this.buffer.push(next, start, nextEnd, 4);
           }
           else { // Shift-and-stay, which means this is a skipped token
               this.pos = nextEnd;
               this.shiftContext(next, start);
               if (next <= this.p.parser.maxNode)
                   this.buffer.push(next, start, nextEnd, 4);
           }
       }
       // Apply an action
       /// @internal
       apply(action, next, nextEnd) {
           if (action & 65536 /* ReduceFlag */)
               this.reduce(action);
           else
               this.shift(action, next, nextEnd);
       }
       // Add a prebuilt (reused) node into the buffer.
       /// @internal
       useNode(value, next) {
           let index = this.p.reused.length - 1;
           if (index < 0 || this.p.reused[index] != value) {
               this.p.reused.push(value);
               index++;
           }
           let start = this.pos;
           this.reducePos = this.pos = start + value.length;
           this.pushState(next, start);
           this.buffer.push(index, start, this.reducePos, -1 /* size == -1 means this is a reused value */);
           if (this.curContext)
               this.updateContext(this.curContext.tracker.reuse(this.curContext.context, value, this, this.p.stream.reset(this.pos - value.length)));
       }
       // Split the stack. Due to the buffer sharing and the fact
       // that `this.stack` tends to stay quite shallow, this isn't very
       // expensive.
       /// @internal
       split() {
           let parent = this;
           let off = parent.buffer.length;
           // Because the top of the buffer (after this.pos) may be mutated
           // to reorder reductions and skipped tokens, and shared buffers
           // should be immutable, this copies any outstanding skipped tokens
           // to the new buffer, and puts the base pointer before them.
           while (off > 0 && parent.buffer[off - 2] > parent.reducePos)
               off -= 4;
           let buffer = parent.buffer.slice(off), base = parent.bufferBase + off;
           // Make sure parent points to an actual parent with content, if there is such a parent.
           while (parent && base == parent.bufferBase)
               parent = parent.parent;
           return new Stack(this.p, this.stack.slice(), this.state, this.reducePos, this.pos, this.score, buffer, base, this.curContext, this.lookAhead, parent);
       }
       // Try to recover from an error by 'deleting' (ignoring) one token.
       /// @internal
       recoverByDelete(next, nextEnd) {
           let isNode = next <= this.p.parser.maxNode;
           if (isNode)
               this.storeNode(next, this.pos, nextEnd, 4);
           this.storeNode(0 /* Err */, this.pos, nextEnd, isNode ? 8 : 4);
           this.pos = this.reducePos = nextEnd;
           this.score -= 190 /* Delete */;
       }
       /// Check if the given term would be able to be shifted (optionally
       /// after some reductions) on this stack. This can be useful for
       /// external tokenizers that want to make sure they only provide a
       /// given token when it applies.
       canShift(term) {
           for (let sim = new SimulatedStack(this);;) {
               let action = this.p.parser.stateSlot(sim.state, 4 /* DefaultReduce */) || this.p.parser.hasAction(sim.state, term);
               if ((action & 65536 /* ReduceFlag */) == 0)
                   return true;
               if (action == 0)
                   return false;
               sim.reduce(action);
           }
       }
       // Apply up to Recover.MaxNext recovery actions that conceptually
       // inserts some missing token or rule.
       /// @internal
       recoverByInsert(next) {
           if (this.stack.length >= 300 /* MaxInsertStackDepth */)
               return [];
           let nextStates = this.p.parser.nextStates(this.state);
           if (nextStates.length > 4 /* MaxNext */ << 1 || this.stack.length >= 120 /* DampenInsertStackDepth */) {
               let best = [];
               for (let i = 0, s; i < nextStates.length; i += 2) {
                   if ((s = nextStates[i + 1]) != this.state && this.p.parser.hasAction(s, next))
                       best.push(nextStates[i], s);
               }
               if (this.stack.length < 120 /* DampenInsertStackDepth */)
                   for (let i = 0; best.length < 4 /* MaxNext */ << 1 && i < nextStates.length; i += 2) {
                       let s = nextStates[i + 1];
                       if (!best.some((v, i) => (i & 1) && v == s))
                           best.push(nextStates[i], s);
                   }
               nextStates = best;
           }
           let result = [];
           for (let i = 0; i < nextStates.length && result.length < 4 /* MaxNext */; i += 2) {
               let s = nextStates[i + 1];
               if (s == this.state)
                   continue;
               let stack = this.split();
               stack.pushState(s, this.pos);
               stack.storeNode(0 /* Err */, stack.pos, stack.pos, 4, true);
               stack.shiftContext(nextStates[i], this.pos);
               stack.score -= 200 /* Insert */;
               result.push(stack);
           }
           return result;
       }
       // Force a reduce, if possible. Return false if that can't
       // be done.
       /// @internal
       forceReduce() {
           let reduce = this.p.parser.stateSlot(this.state, 5 /* ForcedReduce */);
           if ((reduce & 65536 /* ReduceFlag */) == 0)
               return false;
           let { parser } = this.p;
           if (!parser.validAction(this.state, reduce)) {
               let depth = reduce >> 19 /* ReduceDepthShift */, term = reduce & 65535 /* ValueMask */;
               let target = this.stack.length - depth * 3;
               if (target < 0 || parser.getGoto(this.stack[target], term, false) < 0)
                   return false;
               this.storeNode(0 /* Err */, this.reducePos, this.reducePos, 4, true);
               this.score -= 100 /* Reduce */;
           }
           this.reducePos = this.pos;
           this.reduce(reduce);
           return true;
       }
       /// @internal
       forceAll() {
           while (!this.p.parser.stateFlag(this.state, 2 /* Accepting */)) {
               if (!this.forceReduce()) {
                   this.storeNode(0 /* Err */, this.pos, this.pos, 4, true);
                   break;
               }
           }
           return this;
       }
       /// Check whether this state has no further actions (assumed to be a direct descendant of the
       /// top state, since any other states must be able to continue
       /// somehow). @internal
       get deadEnd() {
           if (this.stack.length != 3)
               return false;
           let { parser } = this.p;
           return parser.data[parser.stateSlot(this.state, 1 /* Actions */)] == 65535 /* End */ &&
               !parser.stateSlot(this.state, 4 /* DefaultReduce */);
       }
       /// Restart the stack (put it back in its start state). Only safe
       /// when this.stack.length == 3 (state is directly below the top
       /// state). @internal
       restart() {
           this.state = this.stack[0];
           this.stack.length = 0;
       }
       /// @internal
       sameState(other) {
           if (this.state != other.state || this.stack.length != other.stack.length)
               return false;
           for (let i = 0; i < this.stack.length; i += 3)
               if (this.stack[i] != other.stack[i])
                   return false;
           return true;
       }
       /// Get the parser used by this stack.
       get parser() { return this.p.parser; }
       /// Test whether a given dialect (by numeric ID, as exported from
       /// the terms file) is enabled.
       dialectEnabled(dialectID) { return this.p.parser.dialect.flags[dialectID]; }
       shiftContext(term, start) {
           if (this.curContext)
               this.updateContext(this.curContext.tracker.shift(this.curContext.context, term, this, this.p.stream.reset(start)));
       }
       reduceContext(term, start) {
           if (this.curContext)
               this.updateContext(this.curContext.tracker.reduce(this.curContext.context, term, this, this.p.stream.reset(start)));
       }
       /// @internal
       emitContext() {
           let last = this.buffer.length - 1;
           if (last < 0 || this.buffer[last] != -3)
               this.buffer.push(this.curContext.hash, this.reducePos, this.reducePos, -3);
       }
       /// @internal
       emitLookAhead() {
           let last = this.buffer.length - 1;
           if (last < 0 || this.buffer[last] != -4)
               this.buffer.push(this.lookAhead, this.reducePos, this.reducePos, -4);
       }
       updateContext(context) {
           if (context != this.curContext.context) {
               let newCx = new StackContext(this.curContext.tracker, context);
               if (newCx.hash != this.curContext.hash)
                   this.emitContext();
               this.curContext = newCx;
           }
       }
       /// @internal
       setLookAhead(lookAhead) {
           if (lookAhead > this.lookAhead) {
               this.emitLookAhead();
               this.lookAhead = lookAhead;
           }
       }
       /// @internal
       close() {
           if (this.curContext && this.curContext.tracker.strict)
               this.emitContext();
           if (this.lookAhead > 0)
               this.emitLookAhead();
       }
   }
   class StackContext {
       constructor(tracker, context) {
           this.tracker = tracker;
           this.context = context;
           this.hash = tracker.strict ? tracker.hash(context) : 0;
       }
   }
   var Recover;
   (function (Recover) {
       Recover[Recover["Insert"] = 200] = "Insert";
       Recover[Recover["Delete"] = 190] = "Delete";
       Recover[Recover["Reduce"] = 100] = "Reduce";
       Recover[Recover["MaxNext"] = 4] = "MaxNext";
       Recover[Recover["MaxInsertStackDepth"] = 300] = "MaxInsertStackDepth";
       Recover[Recover["DampenInsertStackDepth"] = 120] = "DampenInsertStackDepth";
   })(Recover || (Recover = {}));
   // Used to cheaply run some reductions to scan ahead without mutating
   // an entire stack
   class SimulatedStack {
       constructor(start) {
           this.start = start;
           this.state = start.state;
           this.stack = start.stack;
           this.base = this.stack.length;
       }
       reduce(action) {
           let term = action & 65535 /* ValueMask */, depth = action >> 19 /* ReduceDepthShift */;
           if (depth == 0) {
               if (this.stack == this.start.stack)
                   this.stack = this.stack.slice();
               this.stack.push(this.state, 0, 0);
               this.base += 3;
           }
           else {
               this.base -= (depth - 1) * 3;
           }
           let goto = this.start.p.parser.getGoto(this.stack[this.base - 3], term, true);
           this.state = goto;
       }
   }
   // This is given to `Tree.build` to build a buffer, and encapsulates
   // the parent-stack-walking necessary to read the nodes.
   class StackBufferCursor {
       constructor(stack, pos, index) {
           this.stack = stack;
           this.pos = pos;
           this.index = index;
           this.buffer = stack.buffer;
           if (this.index == 0)
               this.maybeNext();
       }
       static create(stack, pos = stack.bufferBase + stack.buffer.length) {
           return new StackBufferCursor(stack, pos, pos - stack.bufferBase);
       }
       maybeNext() {
           let next = this.stack.parent;
           if (next != null) {
               this.index = this.stack.bufferBase - next.bufferBase;
               this.stack = next;
               this.buffer = next.buffer;
           }
       }
       get id() { return this.buffer[this.index - 4]; }
       get start() { return this.buffer[this.index - 3]; }
       get end() { return this.buffer[this.index - 2]; }
       get size() { return this.buffer[this.index - 1]; }
       next() {
           this.index -= 4;
           this.pos -= 4;
           if (this.index == 0)
               this.maybeNext();
       }
       fork() {
           return new StackBufferCursor(this.stack, this.pos, this.index);
       }
   }

   class CachedToken {
       constructor() {
           this.start = -1;
           this.value = -1;
           this.end = -1;
           this.extended = -1;
           this.lookAhead = 0;
           this.mask = 0;
           this.context = 0;
       }
   }
   const nullToken = new CachedToken;
   /// [Tokenizers](#lr.ExternalTokenizer) interact with the input
   /// through this interface. It presents the input as a stream of
   /// characters, tracking lookahead and hiding the complexity of
   /// [ranges](#common.Parser.parse^ranges) from tokenizer code.
   class InputStream {
       /// @internal
       constructor(
       /// @internal
       input, 
       /// @internal
       ranges) {
           this.input = input;
           this.ranges = ranges;
           /// @internal
           this.chunk = "";
           /// @internal
           this.chunkOff = 0;
           /// Backup chunk
           this.chunk2 = "";
           this.chunk2Pos = 0;
           /// The character code of the next code unit in the input, or -1
           /// when the stream is at the end of the input.
           this.next = -1;
           /// @internal
           this.token = nullToken;
           this.rangeIndex = 0;
           this.pos = this.chunkPos = ranges[0].from;
           this.range = ranges[0];
           this.end = ranges[ranges.length - 1].to;
           this.readNext();
       }
       /// @internal
       resolveOffset(offset, assoc) {
           let range = this.range, index = this.rangeIndex;
           let pos = this.pos + offset;
           while (pos < range.from) {
               if (!index)
                   return null;
               let next = this.ranges[--index];
               pos -= range.from - next.to;
               range = next;
           }
           while (assoc < 0 ? pos > range.to : pos >= range.to) {
               if (index == this.ranges.length - 1)
                   return null;
               let next = this.ranges[++index];
               pos += next.from - range.to;
               range = next;
           }
           return pos;
       }
       /// @internal
       clipPos(pos) {
           if (pos >= this.range.from && pos < this.range.to)
               return pos;
           for (let range of this.ranges)
               if (range.to > pos)
                   return Math.max(pos, range.from);
           return this.end;
       }
       /// Look at a code unit near the stream position. `.peek(0)` equals
       /// `.next`, `.peek(-1)` gives you the previous character, and so
       /// on.
       ///
       /// Note that looking around during tokenizing creates dependencies
       /// on potentially far-away content, which may reduce the
       /// effectiveness incremental parsing—when looking forward—or even
       /// cause invalid reparses when looking backward more than 25 code
       /// units, since the library does not track lookbehind.
       peek(offset) {
           let idx = this.chunkOff + offset, pos, result;
           if (idx >= 0 && idx < this.chunk.length) {
               pos = this.pos + offset;
               result = this.chunk.charCodeAt(idx);
           }
           else {
               let resolved = this.resolveOffset(offset, 1);
               if (resolved == null)
                   return -1;
               pos = resolved;
               if (pos >= this.chunk2Pos && pos < this.chunk2Pos + this.chunk2.length) {
                   result = this.chunk2.charCodeAt(pos - this.chunk2Pos);
               }
               else {
                   let i = this.rangeIndex, range = this.range;
                   while (range.to <= pos)
                       range = this.ranges[++i];
                   this.chunk2 = this.input.chunk(this.chunk2Pos = pos);
                   if (pos + this.chunk2.length > range.to)
                       this.chunk2 = this.chunk2.slice(0, range.to - pos);
                   result = this.chunk2.charCodeAt(0);
               }
           }
           if (pos >= this.token.lookAhead)
               this.token.lookAhead = pos + 1;
           return result;
       }
       /// Accept a token. By default, the end of the token is set to the
       /// current stream position, but you can pass an offset (relative to
       /// the stream position) to change that.
       acceptToken(token, endOffset = 0) {
           let end = endOffset ? this.resolveOffset(endOffset, -1) : this.pos;
           if (end == null || end < this.token.start)
               throw new RangeError("Token end out of bounds");
           this.token.value = token;
           this.token.end = end;
       }
       getChunk() {
           if (this.pos >= this.chunk2Pos && this.pos < this.chunk2Pos + this.chunk2.length) {
               let { chunk, chunkPos } = this;
               this.chunk = this.chunk2;
               this.chunkPos = this.chunk2Pos;
               this.chunk2 = chunk;
               this.chunk2Pos = chunkPos;
               this.chunkOff = this.pos - this.chunkPos;
           }
           else {
               this.chunk2 = this.chunk;
               this.chunk2Pos = this.chunkPos;
               let nextChunk = this.input.chunk(this.pos);
               let end = this.pos + nextChunk.length;
               this.chunk = end > this.range.to ? nextChunk.slice(0, this.range.to - this.pos) : nextChunk;
               this.chunkPos = this.pos;
               this.chunkOff = 0;
           }
       }
       readNext() {
           if (this.chunkOff >= this.chunk.length) {
               this.getChunk();
               if (this.chunkOff == this.chunk.length)
                   return this.next = -1;
           }
           return this.next = this.chunk.charCodeAt(this.chunkOff);
       }
       /// Move the stream forward N (defaults to 1) code units. Returns
       /// the new value of [`next`](#lr.InputStream.next).
       advance(n = 1) {
           this.chunkOff += n;
           while (this.pos + n >= this.range.to) {
               if (this.rangeIndex == this.ranges.length - 1)
                   return this.setDone();
               n -= this.range.to - this.pos;
               this.range = this.ranges[++this.rangeIndex];
               this.pos = this.range.from;
           }
           this.pos += n;
           if (this.pos >= this.token.lookAhead)
               this.token.lookAhead = this.pos + 1;
           return this.readNext();
       }
       setDone() {
           this.pos = this.chunkPos = this.end;
           this.range = this.ranges[this.rangeIndex = this.ranges.length - 1];
           this.chunk = "";
           return this.next = -1;
       }
       /// @internal
       reset(pos, token) {
           if (token) {
               this.token = token;
               token.start = pos;
               token.lookAhead = pos + 1;
               token.value = token.extended = -1;
           }
           else {
               this.token = nullToken;
           }
           if (this.pos != pos) {
               this.pos = pos;
               if (pos == this.end) {
                   this.setDone();
                   return this;
               }
               while (pos < this.range.from)
                   this.range = this.ranges[--this.rangeIndex];
               while (pos >= this.range.to)
                   this.range = this.ranges[++this.rangeIndex];
               if (pos >= this.chunkPos && pos < this.chunkPos + this.chunk.length) {
                   this.chunkOff = pos - this.chunkPos;
               }
               else {
                   this.chunk = "";
                   this.chunkOff = 0;
               }
               this.readNext();
           }
           return this;
       }
       /// @internal
       read(from, to) {
           if (from >= this.chunkPos && to <= this.chunkPos + this.chunk.length)
               return this.chunk.slice(from - this.chunkPos, to - this.chunkPos);
           if (from >= this.chunk2Pos && to <= this.chunk2Pos + this.chunk2.length)
               return this.chunk2.slice(from - this.chunk2Pos, to - this.chunk2Pos);
           if (from >= this.range.from && to <= this.range.to)
               return this.input.read(from, to);
           let result = "";
           for (let r of this.ranges) {
               if (r.from >= to)
                   break;
               if (r.to > from)
                   result += this.input.read(Math.max(r.from, from), Math.min(r.to, to));
           }
           return result;
       }
   }
   /// @internal
   class TokenGroup {
       constructor(data, id) {
           this.data = data;
           this.id = id;
       }
       token(input, stack) { readToken(this.data, input, stack, this.id); }
   }
   TokenGroup.prototype.contextual = TokenGroup.prototype.fallback = TokenGroup.prototype.extend = false;
   /// `@external tokens` declarations in the grammar should resolve to
   /// an instance of this class.
   class ExternalTokenizer {
       /// Create a tokenizer. The first argument is the function that,
       /// given an input stream, scans for the types of tokens it
       /// recognizes at the stream's position, and calls
       /// [`acceptToken`](#lr.InputStream.acceptToken) when it finds
       /// one.
       constructor(
       /// @internal
       token, options = {}) {
           this.token = token;
           this.contextual = !!options.contextual;
           this.fallback = !!options.fallback;
           this.extend = !!options.extend;
       }
   }
   // Tokenizer data is stored a big uint16 array containing, for each
   // state:
   //
   //  - A group bitmask, indicating what token groups are reachable from
   //    this state, so that paths that can only lead to tokens not in
   //    any of the current groups can be cut off early.
   //
   //  - The position of the end of the state's sequence of accepting
   //    tokens
   //
   //  - The number of outgoing edges for the state
   //
   //  - The accepting tokens, as (token id, group mask) pairs
   //
   //  - The outgoing edges, as (start character, end character, state
   //    index) triples, with end character being exclusive
   //
   // This function interprets that data, running through a stream as
   // long as new states with the a matching group mask can be reached,
   // and updating `input.token` when it matches a token.
   function readToken(data, input, stack, group) {
       let state = 0, groupMask = 1 << group, { parser } = stack.p, { dialect } = parser;
       scan: for (;;) {
           if ((groupMask & data[state]) == 0)
               break;
           let accEnd = data[state + 1];
           // Check whether this state can lead to a token in the current group
           // Accept tokens in this state, possibly overwriting
           // lower-precedence / shorter tokens
           for (let i = state + 3; i < accEnd; i += 2)
               if ((data[i + 1] & groupMask) > 0) {
                   let term = data[i];
                   if (dialect.allows(term) &&
                       (input.token.value == -1 || input.token.value == term || parser.overrides(term, input.token.value))) {
                       input.acceptToken(term);
                       break;
                   }
               }
           let next = input.next, low = 0, high = data[state + 2];
           // Special case for EOF
           if (input.next < 0 && high > low && data[accEnd + high * 3 - 3] == 65535 /* End */) {
               state = data[accEnd + high * 3 - 1];
               continue scan;
           }
           // Do a binary search on the state's edges
           for (; low < high;) {
               let mid = (low + high) >> 1;
               let index = accEnd + mid + (mid << 1);
               let from = data[index], to = data[index + 1];
               if (next < from)
                   high = mid;
               else if (next >= to)
                   low = mid + 1;
               else {
                   state = data[index + 2];
                   input.advance();
                   continue scan;
               }
           }
           break;
       }
   }

   // See lezer-generator/src/encode.ts for comments about the encoding
   // used here
   function decodeArray(input, Type = Uint16Array) {
       if (typeof input != "string")
           return input;
       let array = null;
       for (let pos = 0, out = 0; pos < input.length;) {
           let value = 0;
           for (;;) {
               let next = input.charCodeAt(pos++), stop = false;
               if (next == 126 /* BigValCode */) {
                   value = 65535 /* BigVal */;
                   break;
               }
               if (next >= 92 /* Gap2 */)
                   next--;
               if (next >= 34 /* Gap1 */)
                   next--;
               let digit = next - 32 /* Start */;
               if (digit >= 46 /* Base */) {
                   digit -= 46 /* Base */;
                   stop = true;
               }
               value += digit;
               if (stop)
                   break;
               value *= 46 /* Base */;
           }
           if (array)
               array[out++] = value;
           else
               array = new Type(value);
       }
       return array;
   }

   // Environment variable used to control console output
   const verbose = typeof process != "undefined" && process.env && /\bparse\b/.test(process.env.LOG);
   let stackIDs = null;
   var Safety;
   (function (Safety) {
       Safety[Safety["Margin"] = 25] = "Margin";
   })(Safety || (Safety = {}));
   function cutAt(tree, pos, side) {
       let cursor = tree.cursor(IterMode.IncludeAnonymous);
       cursor.moveTo(pos);
       for (;;) {
           if (!(side < 0 ? cursor.childBefore(pos) : cursor.childAfter(pos)))
               for (;;) {
                   if ((side < 0 ? cursor.to < pos : cursor.from > pos) && !cursor.type.isError)
                       return side < 0 ? Math.max(0, Math.min(cursor.to - 1, pos - 25 /* Margin */))
                           : Math.min(tree.length, Math.max(cursor.from + 1, pos + 25 /* Margin */));
                   if (side < 0 ? cursor.prevSibling() : cursor.nextSibling())
                       break;
                   if (!cursor.parent())
                       return side < 0 ? 0 : tree.length;
               }
       }
   }
   class FragmentCursor {
       constructor(fragments, nodeSet) {
           this.fragments = fragments;
           this.nodeSet = nodeSet;
           this.i = 0;
           this.fragment = null;
           this.safeFrom = -1;
           this.safeTo = -1;
           this.trees = [];
           this.start = [];
           this.index = [];
           this.nextFragment();
       }
       nextFragment() {
           let fr = this.fragment = this.i == this.fragments.length ? null : this.fragments[this.i++];
           if (fr) {
               this.safeFrom = fr.openStart ? cutAt(fr.tree, fr.from + fr.offset, 1) - fr.offset : fr.from;
               this.safeTo = fr.openEnd ? cutAt(fr.tree, fr.to + fr.offset, -1) - fr.offset : fr.to;
               while (this.trees.length) {
                   this.trees.pop();
                   this.start.pop();
                   this.index.pop();
               }
               this.trees.push(fr.tree);
               this.start.push(-fr.offset);
               this.index.push(0);
               this.nextStart = this.safeFrom;
           }
           else {
               this.nextStart = 1e9;
           }
       }
       // `pos` must be >= any previously given `pos` for this cursor
       nodeAt(pos) {
           if (pos < this.nextStart)
               return null;
           while (this.fragment && this.safeTo <= pos)
               this.nextFragment();
           if (!this.fragment)
               return null;
           for (;;) {
               let last = this.trees.length - 1;
               if (last < 0) { // End of tree
                   this.nextFragment();
                   return null;
               }
               let top = this.trees[last], index = this.index[last];
               if (index == top.children.length) {
                   this.trees.pop();
                   this.start.pop();
                   this.index.pop();
                   continue;
               }
               let next = top.children[index];
               let start = this.start[last] + top.positions[index];
               if (start > pos) {
                   this.nextStart = start;
                   return null;
               }
               if (next instanceof Tree) {
                   if (start == pos) {
                       if (start < this.safeFrom)
                           return null;
                       let end = start + next.length;
                       if (end <= this.safeTo) {
                           let lookAhead = next.prop(NodeProp.lookAhead);
                           if (!lookAhead || end + lookAhead < this.fragment.to)
                               return next;
                       }
                   }
                   this.index[last]++;
                   if (start + next.length >= Math.max(this.safeFrom, pos)) { // Enter this node
                       this.trees.push(next);
                       this.start.push(start);
                       this.index.push(0);
                   }
               }
               else {
                   this.index[last]++;
                   this.nextStart = start + next.length;
               }
           }
       }
   }
   class TokenCache {
       constructor(parser, stream) {
           this.stream = stream;
           this.tokens = [];
           this.mainToken = null;
           this.actions = [];
           this.tokens = parser.tokenizers.map(_ => new CachedToken);
       }
       getActions(stack) {
           let actionIndex = 0;
           let main = null;
           let { parser } = stack.p, { tokenizers } = parser;
           let mask = parser.stateSlot(stack.state, 3 /* TokenizerMask */);
           let context = stack.curContext ? stack.curContext.hash : 0;
           let lookAhead = 0;
           for (let i = 0; i < tokenizers.length; i++) {
               if (((1 << i) & mask) == 0)
                   continue;
               let tokenizer = tokenizers[i], token = this.tokens[i];
               if (main && !tokenizer.fallback)
                   continue;
               if (tokenizer.contextual || token.start != stack.pos || token.mask != mask || token.context != context) {
                   this.updateCachedToken(token, tokenizer, stack);
                   token.mask = mask;
                   token.context = context;
               }
               if (token.lookAhead > token.end + 25 /* Margin */)
                   lookAhead = Math.max(token.lookAhead, lookAhead);
               if (token.value != 0 /* Err */) {
                   let startIndex = actionIndex;
                   if (token.extended > -1)
                       actionIndex = this.addActions(stack, token.extended, token.end, actionIndex);
                   actionIndex = this.addActions(stack, token.value, token.end, actionIndex);
                   if (!tokenizer.extend) {
                       main = token;
                       if (actionIndex > startIndex)
                           break;
                   }
               }
           }
           while (this.actions.length > actionIndex)
               this.actions.pop();
           if (lookAhead)
               stack.setLookAhead(lookAhead);
           if (!main && stack.pos == this.stream.end) {
               main = new CachedToken;
               main.value = stack.p.parser.eofTerm;
               main.start = main.end = stack.pos;
               actionIndex = this.addActions(stack, main.value, main.end, actionIndex);
           }
           this.mainToken = main;
           return this.actions;
       }
       getMainToken(stack) {
           if (this.mainToken)
               return this.mainToken;
           let main = new CachedToken, { pos, p } = stack;
           main.start = pos;
           main.end = Math.min(pos + 1, p.stream.end);
           main.value = pos == p.stream.end ? p.parser.eofTerm : 0 /* Err */;
           return main;
       }
       updateCachedToken(token, tokenizer, stack) {
           let start = this.stream.clipPos(stack.pos);
           tokenizer.token(this.stream.reset(start, token), stack);
           if (token.value > -1) {
               let { parser } = stack.p;
               for (let i = 0; i < parser.specialized.length; i++)
                   if (parser.specialized[i] == token.value) {
                       let result = parser.specializers[i](this.stream.read(token.start, token.end), stack);
                       if (result >= 0 && stack.p.parser.dialect.allows(result >> 1)) {
                           if ((result & 1) == 0 /* Specialize */)
                               token.value = result >> 1;
                           else
                               token.extended = result >> 1;
                           break;
                       }
                   }
           }
           else {
               token.value = 0 /* Err */;
               token.end = this.stream.clipPos(start + 1);
           }
       }
       putAction(action, token, end, index) {
           // Don't add duplicate actions
           for (let i = 0; i < index; i += 3)
               if (this.actions[i] == action)
                   return index;
           this.actions[index++] = action;
           this.actions[index++] = token;
           this.actions[index++] = end;
           return index;
       }
       addActions(stack, token, end, index) {
           let { state } = stack, { parser } = stack.p, { data } = parser;
           for (let set = 0; set < 2; set++) {
               for (let i = parser.stateSlot(state, set ? 2 /* Skip */ : 1 /* Actions */);; i += 3) {
                   if (data[i] == 65535 /* End */) {
                       if (data[i + 1] == 1 /* Next */) {
                           i = pair(data, i + 2);
                       }
                       else {
                           if (index == 0 && data[i + 1] == 2 /* Other */)
                               index = this.putAction(pair(data, i + 2), token, end, index);
                           break;
                       }
                   }
                   if (data[i] == token)
                       index = this.putAction(pair(data, i + 1), token, end, index);
               }
           }
           return index;
       }
   }
   var Rec;
   (function (Rec) {
       Rec[Rec["Distance"] = 5] = "Distance";
       Rec[Rec["MaxRemainingPerStep"] = 3] = "MaxRemainingPerStep";
       // When two stacks have been running independently long enough to
       // add this many elements to their buffers, prune one.
       Rec[Rec["MinBufferLengthPrune"] = 500] = "MinBufferLengthPrune";
       Rec[Rec["ForceReduceLimit"] = 10] = "ForceReduceLimit";
       // Once a stack reaches this depth (in .stack.length) force-reduce
       // it back to CutTo to avoid creating trees that overflow the stack
       // on recursive traversal.
       Rec[Rec["CutDepth"] = 15000] = "CutDepth";
       Rec[Rec["CutTo"] = 9000] = "CutTo";
   })(Rec || (Rec = {}));
   class Parse {
       constructor(parser, input, fragments, ranges) {
           this.parser = parser;
           this.input = input;
           this.ranges = ranges;
           this.recovering = 0;
           this.nextStackID = 0x2654; // ♔, ♕, ♖, ♗, ♘, ♙, ♠, ♡, ♢, ♣, ♤, ♥, ♦, ♧
           this.minStackPos = 0;
           this.reused = [];
           this.stoppedAt = null;
           this.stream = new InputStream(input, ranges);
           this.tokens = new TokenCache(parser, this.stream);
           this.topTerm = parser.top[1];
           let { from } = ranges[0];
           this.stacks = [Stack.start(this, parser.top[0], from)];
           this.fragments = fragments.length && this.stream.end - from > parser.bufferLength * 4
               ? new FragmentCursor(fragments, parser.nodeSet) : null;
       }
       get parsedPos() {
           return this.minStackPos;
       }
       // Move the parser forward. This will process all parse stacks at
       // `this.pos` and try to advance them to a further position. If no
       // stack for such a position is found, it'll start error-recovery.
       //
       // When the parse is finished, this will return a syntax tree. When
       // not, it returns `null`.
       advance() {
           let stacks = this.stacks, pos = this.minStackPos;
           // This will hold stacks beyond `pos`.
           let newStacks = this.stacks = [];
           let stopped, stoppedTokens;
           // Keep advancing any stacks at `pos` until they either move
           // forward or can't be advanced. Gather stacks that can't be
           // advanced further in `stopped`.
           for (let i = 0; i < stacks.length; i++) {
               let stack = stacks[i];
               for (;;) {
                   this.tokens.mainToken = null;
                   if (stack.pos > pos) {
                       newStacks.push(stack);
                   }
                   else if (this.advanceStack(stack, newStacks, stacks)) {
                       continue;
                   }
                   else {
                       if (!stopped) {
                           stopped = [];
                           stoppedTokens = [];
                       }
                       stopped.push(stack);
                       let tok = this.tokens.getMainToken(stack);
                       stoppedTokens.push(tok.value, tok.end);
                   }
                   break;
               }
           }
           if (!newStacks.length) {
               let finished = stopped && findFinished(stopped);
               if (finished)
                   return this.stackToTree(finished);
               if (this.parser.strict) {
                   if (verbose && stopped)
                       console.log("Stuck with token " + (this.tokens.mainToken ? this.parser.getName(this.tokens.mainToken.value) : "none"));
                   throw new SyntaxError("No parse at " + pos);
               }
               if (!this.recovering)
                   this.recovering = 5 /* Distance */;
           }
           if (this.recovering && stopped) {
               let finished = this.stoppedAt != null && stopped[0].pos > this.stoppedAt ? stopped[0]
                   : this.runRecovery(stopped, stoppedTokens, newStacks);
               if (finished)
                   return this.stackToTree(finished.forceAll());
           }
           if (this.recovering) {
               let maxRemaining = this.recovering == 1 ? 1 : this.recovering * 3 /* MaxRemainingPerStep */;
               if (newStacks.length > maxRemaining) {
                   newStacks.sort((a, b) => b.score - a.score);
                   while (newStacks.length > maxRemaining)
                       newStacks.pop();
               }
               if (newStacks.some(s => s.reducePos > pos))
                   this.recovering--;
           }
           else if (newStacks.length > 1) {
               // Prune stacks that are in the same state, or that have been
               // running without splitting for a while, to avoid getting stuck
               // with multiple successful stacks running endlessly on.
               outer: for (let i = 0; i < newStacks.length - 1; i++) {
                   let stack = newStacks[i];
                   for (let j = i + 1; j < newStacks.length; j++) {
                       let other = newStacks[j];
                       if (stack.sameState(other) ||
                           stack.buffer.length > 500 /* MinBufferLengthPrune */ && other.buffer.length > 500 /* MinBufferLengthPrune */) {
                           if (((stack.score - other.score) || (stack.buffer.length - other.buffer.length)) > 0) {
                               newStacks.splice(j--, 1);
                           }
                           else {
                               newStacks.splice(i--, 1);
                               continue outer;
                           }
                       }
                   }
               }
           }
           this.minStackPos = newStacks[0].pos;
           for (let i = 1; i < newStacks.length; i++)
               if (newStacks[i].pos < this.minStackPos)
                   this.minStackPos = newStacks[i].pos;
           return null;
       }
       stopAt(pos) {
           if (this.stoppedAt != null && this.stoppedAt < pos)
               throw new RangeError("Can't move stoppedAt forward");
           this.stoppedAt = pos;
       }
       // Returns an updated version of the given stack, or null if the
       // stack can't advance normally. When `split` and `stacks` are
       // given, stacks split off by ambiguous operations will be pushed to
       // `split`, or added to `stacks` if they move `pos` forward.
       advanceStack(stack, stacks, split) {
           let start = stack.pos, { parser } = this;
           let base = verbose ? this.stackID(stack) + " -> " : "";
           if (this.stoppedAt != null && start > this.stoppedAt)
               return stack.forceReduce() ? stack : null;
           if (this.fragments) {
               let strictCx = stack.curContext && stack.curContext.tracker.strict, cxHash = strictCx ? stack.curContext.hash : 0;
               for (let cached = this.fragments.nodeAt(start); cached;) {
                   let match = this.parser.nodeSet.types[cached.type.id] == cached.type ? parser.getGoto(stack.state, cached.type.id) : -1;
                   if (match > -1 && cached.length && (!strictCx || (cached.prop(NodeProp.contextHash) || 0) == cxHash)) {
                       stack.useNode(cached, match);
                       if (verbose)
                           console.log(base + this.stackID(stack) + ` (via reuse of ${parser.getName(cached.type.id)})`);
                       return true;
                   }
                   if (!(cached instanceof Tree) || cached.children.length == 0 || cached.positions[0] > 0)
                       break;
                   let inner = cached.children[0];
                   if (inner instanceof Tree && cached.positions[0] == 0)
                       cached = inner;
                   else
                       break;
               }
           }
           let defaultReduce = parser.stateSlot(stack.state, 4 /* DefaultReduce */);
           if (defaultReduce > 0) {
               stack.reduce(defaultReduce);
               if (verbose)
                   console.log(base + this.stackID(stack) + ` (via always-reduce ${parser.getName(defaultReduce & 65535 /* ValueMask */)})`);
               return true;
           }
           if (stack.stack.length >= 15000 /* CutDepth */) {
               while (stack.stack.length > 9000 /* CutTo */ && stack.forceReduce()) { }
           }
           let actions = this.tokens.getActions(stack);
           for (let i = 0; i < actions.length;) {
               let action = actions[i++], term = actions[i++], end = actions[i++];
               let last = i == actions.length || !split;
               let localStack = last ? stack : stack.split();
               localStack.apply(action, term, end);
               if (verbose)
                   console.log(base + this.stackID(localStack) + ` (via ${(action & 65536 /* ReduceFlag */) == 0 ? "shift"
                    : `reduce of ${parser.getName(action & 65535 /* ValueMask */)}`} for ${parser.getName(term)} @ ${start}${localStack == stack ? "" : ", split"})`);
               if (last)
                   return true;
               else if (localStack.pos > start)
                   stacks.push(localStack);
               else
                   split.push(localStack);
           }
           return false;
       }
       // Advance a given stack forward as far as it will go. Returns the
       // (possibly updated) stack if it got stuck, or null if it moved
       // forward and was given to `pushStackDedup`.
       advanceFully(stack, newStacks) {
           let pos = stack.pos;
           for (;;) {
               if (!this.advanceStack(stack, null, null))
                   return false;
               if (stack.pos > pos) {
                   pushStackDedup(stack, newStacks);
                   return true;
               }
           }
       }
       runRecovery(stacks, tokens, newStacks) {
           let finished = null, restarted = false;
           for (let i = 0; i < stacks.length; i++) {
               let stack = stacks[i], token = tokens[i << 1], tokenEnd = tokens[(i << 1) + 1];
               let base = verbose ? this.stackID(stack) + " -> " : "";
               if (stack.deadEnd) {
                   if (restarted)
                       continue;
                   restarted = true;
                   stack.restart();
                   if (verbose)
                       console.log(base + this.stackID(stack) + " (restarted)");
                   let done = this.advanceFully(stack, newStacks);
                   if (done)
                       continue;
               }
               let force = stack.split(), forceBase = base;
               for (let j = 0; force.forceReduce() && j < 10 /* ForceReduceLimit */; j++) {
                   if (verbose)
                       console.log(forceBase + this.stackID(force) + " (via force-reduce)");
                   let done = this.advanceFully(force, newStacks);
                   if (done)
                       break;
                   if (verbose)
                       forceBase = this.stackID(force) + " -> ";
               }
               for (let insert of stack.recoverByInsert(token)) {
                   if (verbose)
                       console.log(base + this.stackID(insert) + " (via recover-insert)");
                   this.advanceFully(insert, newStacks);
               }
               if (this.stream.end > stack.pos) {
                   if (tokenEnd == stack.pos) {
                       tokenEnd++;
                       token = 0 /* Err */;
                   }
                   stack.recoverByDelete(token, tokenEnd);
                   if (verbose)
                       console.log(base + this.stackID(stack) + ` (via recover-delete ${this.parser.getName(token)})`);
                   pushStackDedup(stack, newStacks);
               }
               else if (!finished || finished.score < stack.score) {
                   finished = stack;
               }
           }
           return finished;
       }
       // Convert the stack's buffer to a syntax tree.
       stackToTree(stack) {
           stack.close();
           return Tree.build({ buffer: StackBufferCursor.create(stack),
               nodeSet: this.parser.nodeSet,
               topID: this.topTerm,
               maxBufferLength: this.parser.bufferLength,
               reused: this.reused,
               start: this.ranges[0].from,
               length: stack.pos - this.ranges[0].from,
               minRepeatType: this.parser.minRepeatTerm });
       }
       stackID(stack) {
           let id = (stackIDs || (stackIDs = new WeakMap)).get(stack);
           if (!id)
               stackIDs.set(stack, id = String.fromCodePoint(this.nextStackID++));
           return id + stack;
       }
   }
   function pushStackDedup(stack, newStacks) {
       for (let i = 0; i < newStacks.length; i++) {
           let other = newStacks[i];
           if (other.pos == stack.pos && other.sameState(stack)) {
               if (newStacks[i].score < stack.score)
                   newStacks[i] = stack;
               return;
           }
       }
       newStacks.push(stack);
   }
   class Dialect {
       constructor(source, flags, disabled) {
           this.source = source;
           this.flags = flags;
           this.disabled = disabled;
       }
       allows(term) { return !this.disabled || this.disabled[term] == 0; }
   }
   /// Holds the parse tables for a given grammar, as generated by
   /// `lezer-generator`, and provides [methods](#common.Parser) to parse
   /// content with.
   class LRParser extends Parser {
       /// @internal
       constructor(spec) {
           super();
           /// @internal
           this.wrappers = [];
           if (spec.version != 14 /* Version */)
               throw new RangeError(`Parser version (${spec.version}) doesn't match runtime version (${14 /* Version */})`);
           let nodeNames = spec.nodeNames.split(" ");
           this.minRepeatTerm = nodeNames.length;
           for (let i = 0; i < spec.repeatNodeCount; i++)
               nodeNames.push("");
           let topTerms = Object.keys(spec.topRules).map(r => spec.topRules[r][1]);
           let nodeProps = [];
           for (let i = 0; i < nodeNames.length; i++)
               nodeProps.push([]);
           function setProp(nodeID, prop, value) {
               nodeProps[nodeID].push([prop, prop.deserialize(String(value))]);
           }
           if (spec.nodeProps)
               for (let propSpec of spec.nodeProps) {
                   let prop = propSpec[0];
                   if (typeof prop == "string")
                       prop = NodeProp[prop];
                   for (let i = 1; i < propSpec.length;) {
                       let next = propSpec[i++];
                       if (next >= 0) {
                           setProp(next, prop, propSpec[i++]);
                       }
                       else {
                           let value = propSpec[i + -next];
                           for (let j = -next; j > 0; j--)
                               setProp(propSpec[i++], prop, value);
                           i++;
                       }
                   }
               }
           this.nodeSet = new NodeSet(nodeNames.map((name, i) => NodeType.define({
               name: i >= this.minRepeatTerm ? undefined : name,
               id: i,
               props: nodeProps[i],
               top: topTerms.indexOf(i) > -1,
               error: i == 0,
               skipped: spec.skippedNodes && spec.skippedNodes.indexOf(i) > -1
           })));
           if (spec.propSources)
               this.nodeSet = this.nodeSet.extend(...spec.propSources);
           this.strict = false;
           this.bufferLength = DefaultBufferLength;
           let tokenArray = decodeArray(spec.tokenData);
           this.context = spec.context;
           this.specializerSpecs = spec.specialized || [];
           this.specialized = new Uint16Array(this.specializerSpecs.length);
           for (let i = 0; i < this.specializerSpecs.length; i++)
               this.specialized[i] = this.specializerSpecs[i].term;
           this.specializers = this.specializerSpecs.map(getSpecializer);
           this.states = decodeArray(spec.states, Uint32Array);
           this.data = decodeArray(spec.stateData);
           this.goto = decodeArray(spec.goto);
           this.maxTerm = spec.maxTerm;
           this.tokenizers = spec.tokenizers.map(value => typeof value == "number" ? new TokenGroup(tokenArray, value) : value);
           this.topRules = spec.topRules;
           this.dialects = spec.dialects || {};
           this.dynamicPrecedences = spec.dynamicPrecedences || null;
           this.tokenPrecTable = spec.tokenPrec;
           this.termNames = spec.termNames || null;
           this.maxNode = this.nodeSet.types.length - 1;
           this.dialect = this.parseDialect();
           this.top = this.topRules[Object.keys(this.topRules)[0]];
       }
       createParse(input, fragments, ranges) {
           let parse = new Parse(this, input, fragments, ranges);
           for (let w of this.wrappers)
               parse = w(parse, input, fragments, ranges);
           return parse;
       }
       /// Get a goto table entry @internal
       getGoto(state, term, loose = false) {
           let table = this.goto;
           if (term >= table[0])
               return -1;
           for (let pos = table[term + 1];;) {
               let groupTag = table[pos++], last = groupTag & 1;
               let target = table[pos++];
               if (last && loose)
                   return target;
               for (let end = pos + (groupTag >> 1); pos < end; pos++)
                   if (table[pos] == state)
                       return target;
               if (last)
                   return -1;
           }
       }
       /// Check if this state has an action for a given terminal @internal
       hasAction(state, terminal) {
           let data = this.data;
           for (let set = 0; set < 2; set++) {
               for (let i = this.stateSlot(state, set ? 2 /* Skip */ : 1 /* Actions */), next;; i += 3) {
                   if ((next = data[i]) == 65535 /* End */) {
                       if (data[i + 1] == 1 /* Next */)
                           next = data[i = pair(data, i + 2)];
                       else if (data[i + 1] == 2 /* Other */)
                           return pair(data, i + 2);
                       else
                           break;
                   }
                   if (next == terminal || next == 0 /* Err */)
                       return pair(data, i + 1);
               }
           }
           return 0;
       }
       /// @internal
       stateSlot(state, slot) {
           return this.states[(state * 6 /* Size */) + slot];
       }
       /// @internal
       stateFlag(state, flag) {
           return (this.stateSlot(state, 0 /* Flags */) & flag) > 0;
       }
       /// @internal
       validAction(state, action) {
           if (action == this.stateSlot(state, 4 /* DefaultReduce */))
               return true;
           for (let i = this.stateSlot(state, 1 /* Actions */);; i += 3) {
               if (this.data[i] == 65535 /* End */) {
                   if (this.data[i + 1] == 1 /* Next */)
                       i = pair(this.data, i + 2);
                   else
                       return false;
               }
               if (action == pair(this.data, i + 1))
                   return true;
           }
       }
       /// Get the states that can follow this one through shift actions or
       /// goto jumps. @internal
       nextStates(state) {
           let result = [];
           for (let i = this.stateSlot(state, 1 /* Actions */);; i += 3) {
               if (this.data[i] == 65535 /* End */) {
                   if (this.data[i + 1] == 1 /* Next */)
                       i = pair(this.data, i + 2);
                   else
                       break;
               }
               if ((this.data[i + 2] & (65536 /* ReduceFlag */ >> 16)) == 0) {
                   let value = this.data[i + 1];
                   if (!result.some((v, i) => (i & 1) && v == value))
                       result.push(this.data[i], value);
               }
           }
           return result;
       }
       /// @internal
       overrides(token, prev) {
           let iPrev = findOffset(this.data, this.tokenPrecTable, prev);
           return iPrev < 0 || findOffset(this.data, this.tokenPrecTable, token) < iPrev;
       }
       /// Configure the parser. Returns a new parser instance that has the
       /// given settings modified. Settings not provided in `config` are
       /// kept from the original parser.
       configure(config) {
           // Hideous reflection-based kludge to make it easy to create a
           // slightly modified copy of a parser.
           let copy = Object.assign(Object.create(LRParser.prototype), this);
           if (config.props)
               copy.nodeSet = this.nodeSet.extend(...config.props);
           if (config.top) {
               let info = this.topRules[config.top];
               if (!info)
                   throw new RangeError(`Invalid top rule name ${config.top}`);
               copy.top = info;
           }
           if (config.tokenizers)
               copy.tokenizers = this.tokenizers.map(t => {
                   let found = config.tokenizers.find(r => r.from == t);
                   return found ? found.to : t;
               });
           if (config.specializers) {
               copy.specializers = this.specializers.slice();
               copy.specializerSpecs = this.specializerSpecs.map((s, i) => {
                   let found = config.specializers.find(r => r.from == s.external);
                   if (!found)
                       return s;
                   let spec = Object.assign(Object.assign({}, s), { external: found.to });
                   copy.specializers[i] = getSpecializer(spec);
                   return spec;
               });
           }
           if (config.contextTracker)
               copy.context = config.contextTracker;
           if (config.dialect)
               copy.dialect = this.parseDialect(config.dialect);
           if (config.strict != null)
               copy.strict = config.strict;
           if (config.wrap)
               copy.wrappers = copy.wrappers.concat(config.wrap);
           if (config.bufferLength != null)
               copy.bufferLength = config.bufferLength;
           return copy;
       }
       /// Tells you whether any [parse wrappers](#lr.ParserConfig.wrap)
       /// are registered for this parser.
       hasWrappers() {
           return this.wrappers.length > 0;
       }
       /// Returns the name associated with a given term. This will only
       /// work for all terms when the parser was generated with the
       /// `--names` option. By default, only the names of tagged terms are
       /// stored.
       getName(term) {
           return this.termNames ? this.termNames[term] : String(term <= this.maxNode && this.nodeSet.types[term].name || term);
       }
       /// The eof term id is always allocated directly after the node
       /// types. @internal
       get eofTerm() { return this.maxNode + 1; }
       /// The type of top node produced by the parser.
       get topNode() { return this.nodeSet.types[this.top[1]]; }
       /// @internal
       dynamicPrecedence(term) {
           let prec = this.dynamicPrecedences;
           return prec == null ? 0 : prec[term] || 0;
       }
       /// @internal
       parseDialect(dialect) {
           let values = Object.keys(this.dialects), flags = values.map(() => false);
           if (dialect)
               for (let part of dialect.split(" ")) {
                   let id = values.indexOf(part);
                   if (id >= 0)
                       flags[id] = true;
               }
           let disabled = null;
           for (let i = 0; i < values.length; i++)
               if (!flags[i]) {
                   for (let j = this.dialects[values[i]], id; (id = this.data[j++]) != 65535 /* End */;)
                       (disabled || (disabled = new Uint8Array(this.maxTerm + 1)))[id] = 1;
               }
           return new Dialect(dialect, flags, disabled);
       }
       /// Used by the output of the parser generator. Not available to
       /// user code.
       static deserialize(spec) {
           return new LRParser(spec);
       }
   }
   function pair(data, off) { return data[off] | (data[off + 1] << 16); }
   function findOffset(data, start, term) {
       for (let i = start, next; (next = data[i]) != 65535 /* End */; i++)
           if (next == term)
               return i - start;
       return -1;
   }
   function findFinished(stacks) {
       let best = null;
       for (let stack of stacks) {
           let stopped = stack.p.stoppedAt;
           if ((stack.pos == stack.p.stream.end || stopped != null && stack.pos > stopped) &&
               stack.p.parser.stateFlag(stack.state, 2 /* Accepting */) &&
               (!best || best.score < stack.score))
               best = stack;
       }
       return best;
   }
   function getSpecializer(spec) {
       if (spec.external) {
           let mask = spec.extend ? 1 /* Extend */ : 0 /* Specialize */;
           return (value, stack) => (spec.external(value, stack) << 1) | mask;
       }
       return spec.get;
   }

   // This file was generated by lezer-generator. You probably shouldn't edit it.
   const RawString = 1,
     templateArgsEndFallback = 2,
     MacroName = 3;

   const R = 82, L = 76, u = 117, U = 85,
         a = 97, z = 122, A = 65, Z = 90, Underscore = 95,
         Zero = 48,
         Quote = 34,
         ParenL = 40, ParenR = 41,
         Space = 32, GreaterThan = 62;

   const rawString = new ExternalTokenizer(input => {
     // Raw string literals can start with: R, LR, uR, UR, u8R
     if (input.next == L || input.next == U) {
       input.advance();
     } else if (input.next == u) {
       input.advance();
       if (input.next == Zero + 8) input.advance();
     }
     if (input.next != R) return
     input.advance();
     if (input.next != Quote) return
     input.advance();

     let marker = "";
     while (input.next != ParenL) {
       if (input.next == Space || input.next <= 13 || input.next == ParenR) return
       marker += String.fromCharCode(input.next);
       input.advance();
     }
     input.advance();

     for (;;) {
       if (input.next < 0)
         return input.acceptToken(RawString)
       if (input.next == ParenR) {
         let match = true;
         for (let i = 0; match && i < marker.length; i++)
           if (input.peek(i + 1) != marker.charCodeAt(i)) match = false;
         if (match && input.peek(marker.length + 1) == Quote)
           return input.acceptToken(RawString, 2 + marker.length)
       }
       input.advance();
     }
   });

   const fallback = new ExternalTokenizer(input => {
     if (input.next == GreaterThan) {
       // Provide a template-args-closing token when the next characters
       // are ">>", in which case the regular tokenizer will only see a
       // bit shift op.
       if (input.peek(1) == GreaterThan)
         input.acceptToken(templateArgsEndFallback, 1);
     } else {
       // Notice all-uppercase identifiers
       let sawLetter = false, i = 0;
       for (;; i++) {
         if (input.next >= A && input.next <= Z) sawLetter = true;
         else if (input.next >= a && input.next <= z) return
         else if (input.next != Underscore && !(input.next >= Zero && input.next <= Zero + 9)) break
         input.advance();
       }
       if (sawLetter && i > 1) input.acceptToken(MacroName);
     }
   }, {extend: true});

   const cppHighlighting = styleTags({
     "typedef struct union enum class typename decltype auto template operator friend noexcept namespace using __attribute__ __declspec __based": tags.definitionKeyword,
     "extern MsCallModifier MsPointerModifier extern static register inline const volatile restrict _Atomic mutable constexpr virtual explicit VirtualSpecifier Access": tags.modifier,
     "if else switch for while do case default return break continue goto throw try catch": tags.controlKeyword,
     "new sizeof delete static_assert": tags.operatorKeyword,
     "NULL nullptr": tags.null,
     this: tags.self,
     "True False": tags.bool,
     "TypeSize PrimitiveType": tags.standard(tags.typeName),
     TypeIdentifier: tags.typeName,
     FieldIdentifier: tags.propertyName,
     "CallExpression/FieldExpression/FieldIdentifier": tags.function(tags.propertyName),
     StatementIdentifier: tags.labelName,
     "Identifier DestructorName": tags.variableName,
     "CallExpression/Identifier": tags.function(tags.variableName),
     "CallExpression/ScopedIdentifier/Identifier": tags.function(tags.variableName),
     "FunctionDeclarator/Identifier FunctionDeclarator/DestructorName": tags.function(tags.definition(tags.variableName)),
     NamespaceIdentifier: tags.namespace,
     OperatorName: tags.operator,
     ArithOp: tags.arithmeticOperator,
     LogicOp: tags.logicOperator,
     BitOp: tags.bitwiseOperator,
     CompareOp: tags.compareOperator,
     AssignOp: tags.definitionOperator,
     UpdateOp: tags.updateOperator,
     LineComment: tags.lineComment,
     BlockComment: tags.blockComment,
     Number: tags.number,
     String: tags.string,
     "RawString SystemLibString": tags.special(tags.string),
     CharLiteral: tags.character,
     EscapeSequence: tags.escape,
     PreProcArg: tags.meta,
     "PreprocDirectiveName #include #ifdef #ifndef #if #define #else #endif #elif": tags.processingInstruction,
     MacroName: tags.special(tags.name),
     "( )": tags.paren,
     "[ ]": tags.squareBracket,
     "{ }": tags.brace,
     "< >": tags.angleBracket,
     ". ->": tags.derefOperator,
     ", ;": tags.separator
   });

   // This file was generated by lezer-generator. You probably shouldn't edit it.
   const spec_identifier = {__proto__:null,bool:34, char:34, int:34, float:34, double:34, void:34, size_t:34, ssize_t:34, intptr_t:34, uintptr_t:34, charptr_t:34, int8_t:34, int16_t:34, int32_t:34, int64_t:34, uint8_t:34, uint16_t:34, uint32_t:34, uint64_t:34, char8_t:34, char16_t:34, char32_t:34, char64_t:34, const:68, volatile:70, restrict:72, _Atomic:74, mutable:76, constexpr:78, struct:82, __declspec:86, final:90, override:90, public:94, private:94, protected:94, virtual:154, extern:156, static:158, register:160, inline:162, __attribute__:166, __based:172, __restrict:174, __uptr:174, __sptr:174, _unaligned:174, __unaligned:174, noexcept:188, throw:192, new:228, delete:230, operator:236, template:266, typename:272, class:274, using:284, friend:292, __cdecl:296, __clrcall:296, __stdcall:296, __fastcall:296, __thiscall:296, __vectorcall:296, case:306, default:308, if:320, else:326, switch:330, do:334, while:336, for:344, return:348, break:352, continue:356, goto:360, typedef:364, try:378, catch:382, namespace:388, static_assert:394, explicit:404, union:420, enum:442, signed:446, unsigned:446, long:446, short:446, decltype:458, auto:460, sizeof:492, TRUE:746, true:746, FALSE:748, false:748, NULL:500, nullptr:518, this:520};
   const spec_ = {__proto__:null,"<":139};
   const spec_templateArgsEnd = {__proto__:null,">":143};
   const spec_scopedIdentifier = {__proto__:null,operator:218, new:504, delete:510};
   const parser = LRParser.deserialize({
     version: 14,
     states: "$+^Q!QQVOOP&qOUOOO'cOWO'#CdO*|QUO'#CgO+WQUO'#FoO,nQbO'#CwO-PQUO'#CwO.oQUO'#JaO.vQUO'#CvO/ROpO'#DyO/ZQ!dO'#DbOOQQ'#I['#I[O/fQUO'#KOO1VQUO'#I`OOQQ'#I`'#I`O4XQUO'#JrO7YQUO'#JrO9aQVO'#EZO9qQUO'#EZO9vQUOOO:OQVO'#EhO<`QVO'#EiO<pQUO'#EiO?[QUO'#JvO?fQUO'#JvO?pQVO'#HwOOQQ'#Jv'#JvOAdQUO'#JvOCiQUO'#JvOCvQUO'#FaODhQUO'#FmOOQR'#Fo'#FoO,iQUO'#FoOOQR'#KS'#KSOOQR'#KR'#KRODmQVO'#JWOFeQUO'#KVOFrQUO'#KVOOQR'#Gl'#GlOGdQUO'#KXOOQR'#Gs'#GsOOQR'#Gu'#GuOOQR'#KQ'#KQOOQR'#Io'#IoQ!QQVOOOGoQVO'#FgOOQQ'#Iv'#IvOGvQUO'#HSOOQQ'#Ja'#JaOK_QUO'#JaOK{QUO'#JWOM{QVO'#HfOM{QVO'#HiOOQQ'#Hn'#HnOOQQ'#Ho'#HoO! zQUO'#HvOOQQ'#JW'#JWO!$OQUO'#JaOOQR'#J`'#J`O!$VQUO'#DWO!$[QUO'#JsOOQQ'#Js'#JsO!&hQUO'#EQO!&mQUO'#ETO!&rQ#vO'#EuO!'lQUO'#GgO!'wQUO'#FqO!(PQUO'#FvO!(UQVO'#FxO!(PQUO'#F|O!*qQUO'#F}O!*vQVO'#GPO!+QQUO'#GRO!+VQUO'#GTO!+[QUO'#GVO!+aQUO'#GXOM{QVO'#FjO!,hQUO'#FjO!,mQUO'#G`O!,rQVO'#GdO!,yQUO'#GeO!-RQUO'#GhO!/QQUO'#GkO!/bQUO'#GmO4XQUO'#FeO!0SQUO'#DUO!0hQUO'#HPO!0|QUO'#HQO!1_QUO'#HWO!1sQUO'#HXO!2hQUO'#HYO!4WQVO'#HkO!4bQUO'#HqO!4iQVO'#HtP!6V{&jO'#CbP!,|{&jO'#CbP!6b{,UO'#CbP!,|{&jO'#CbP!6g{&jO'#CbP!6rOSO'#IUPOOO)CDt)CDtOOOO'#IW'#IWO!6|OWO,59OOOQR,59O,59OOM{QVO,59UOOQQ,59W,59WOM{QVO,5<[OOQR,5<Z,5<ZO!7XQUO,59YOM{QVO,5={OOQR'#Hc'#HcOOQR'#Hd'#HdOOQR'#He'#HeOM{QVO,5=|OM{QVO,5=|OM{QVO,5=|OM{QVO,5=|OM{QVO,5=|OM{QVO,5=|OM{QVO,5=|OM{QVO,5=|OM{QVO,5=|O!9TQVO,5>TOOQQ,5>d,5>dO!:pQVO'#ChO!>YQUO'#CyOOQQ,59c,59cOOQQ,59b,59bOOQQ,5;U,5;UO!>gQ#vO,5=`O!4bQUO,5>]O!@zQVO,5>`O!ARQbO,59cO!A^QVO'#FQOOQQ,5>X,5>XO!AnQVO,59VO!AuO`O,5:eO!AzQbO'#DcO!B]QbO'#JgO!BkQbO,59|O!DmQUO'#CsO!F]QbO'#CwO!FbQUO'#CvO!IuQUO'#JaOOQQ-E<Y-E<YO!I|QUO,5@jOCvQUO,5;{O!JTQUO'#JaO4XQUO,5<POOQQ-E<^-E<^O!J_QUO,5@^O!J_QUO,5@^OOQR,5@j,5@jO!N_QUO'#CgO# TQUO,5=zO#!vQbO'#CwO##XQUO'#EjO##^QUO'#HmO##fQUO'#CvOOQR'#IX'#IXO##qQUO,5:uO#$XQUO,5:uO#$rQUO,5:uO#%dQUO'#CtO!&mQUO'#ClOOQQ'#Ia'#IaO#%lQ#vO'#EuO#$XQUO,5:uO#'XQUO,5:vO#)xQVO,5;RO#*SQUO'#EXO#*aQUO,5<gO#,eQUO'#JaO!+aQUO'#C}O#,lQUO,5;SO#,wQUO,5<gO#,|QUO'#C}O#-vQUO,5>UO#-{QUO,5;TO#.mQbO'#CwO#$XQUO'#EZO<pQUO'#EhO#.xQUO'#JvO!,|QUO'#EwO#/mQUO'#CtOOQQ,5;`,5;`O#2[QVO'#C{O#2fQUO'#KcO#5cQUO'#JzO#5jQUO'#JWO#5}QUO,5;cO#6SQVO'#F{OM{QVO'#H`O#8lQVO,5>cO?pQVO'#HwO#8vQUO,5>cO#8yQUO,5>cOOQQ,5>c,5>cO#9OQUO'#GoOOQR,5@q,5@qO#9WQUO,5@qO#9`QUO'#GqO#9hQUO,5<PO#9|QUO'#KPO#:UQUO,5;{O!(UQVO,5<XOOQR'#Is'#IsO#:ZQVO,5=UO#<aQbO'#CwO#<lQUO'#EjOAdQUO,5@qOOQQ'#It'#ItOOQQ-E<r-E<rOFrQUO,5@qO#9WQUO,5@sO#<qQUO,5@sOOQR,5@s,5@sOOQR,5@t,5@tO#<|QVO,5@tOOQR-E<m-E<mO#>mQVO,5<ROOQR,5<R,5<ROOQQ-E<t-E<tOOQQ,5=n,5=nOOQQ,59`,59`O#>tQUO,5>QO#@tQUO'#JWO#@{QUO,5>TO#A`QUO'#EbO#B}QUO'#EcO#CqQUO'#EcO#CyQVO'#EdO#DTQUO'#EeO#DqQUO'#EfOOQQ'#Jx'#JxO#E_QUO,5>bOOQQ,5>b,5>bO!,|QUO,59rO#EjQUO,5<SO!&mQUO,5:lOOQQ,5:o,5:oO#EwQUO'#JrOOQQ,5;a,5;aO#HeQUO,5;aO#HoQUO,5;aO#HtQUO,5;aO#HyQUO,5;aO! iQUO,5=`O#ITQUO,5;yO#I]QUO,5=RO#IbQUO,5=RO#LiQVO'#FsO!(UQVO,5<]O!(PQUO,5<]O!,mQUO,5<bO#LpQUO,5<dO!(UQVO,5<hO#LuQVO,5<iO$ wQUO,5<kOOQR,5<k,5<kO$#QQUO,5<kOOQR,5<m,5<mOOQR,5<o,5<oOOQQ'#Fn'#FnO$#VQUO,5<qO!+aQUO,5<sO$#[QUO,5<sO$#jQUO,5<UO$$pQVO,5<UO$&oQUO,5<zO$&tQUO,5=OOOQR,5=O,5=OOOQQ'#Cg'#CgO$'zQVO'#FiOOQR,5=P,5=PO$(RQUO,5=POM{QVO,5=SO$)kQUO'#JaO$)rQUO,5=VO$+nQUO'#FWO<pQUO,5=VO$+xQUO,5;pOM{QVO,5=XOCvQUO,5<PO$/zQUO'#JbO$0RQUO'#JbO$0]QUO'#D[O$0hQUO'#D_O$3UQUO,59pOOQQ,59p,59pO$4zQUO,59pO$5PQUO,59pO$5XQUO,59pO$7ZQUO,5=kOOQQ,5=k,5=kO$4zQUO,5=kO$5PQUO,5=kO$9PQUO,5=kO$9bQUO'#HUO$9jQUO,5=lOOQQ,5=l,5=lO$<{QUO,5=lO$>wQUO,5=rOOQQ,5=r,5=rO$4zQUO,5=rO$5PQUO,5=rO$@mQUO,5=rOOQQ,5=s,5=sOM{QVO,5=tO$AOQUO,5>VO#6SQVO'#F{OOQQ,5>V,5>VO$BqQUO,5>VO$BvQUO,5>]O!1sQUO,5>]O$DyQUO,5>`O$H]QVO,5>`P!6g{&jO,58|P$Hd{&jO,58|P$Hr{,UO,58|P$Hx{&jO,58|PO{O'#I{'#I{P$H}{&jO'#KdPOOO'#Kd'#KdP$IT{&jO'#KdPOOO,58|,58|POOO,5>p,5>pP$IYOSO,5>pOOOO-E<U-E<UOOQR1G.j1G.jO$IaQUO1G.pO$JgQUO1G1vOOQQ1G1v1G1vO$KsQUO'#CoO$NPQbO'#CwO$N[QUO'#CrO$NaQUO1G.tO#/mQUO'#CqOOQQ1G.t1G.tO%!dQUO1G3gO%#jQUO1G3hO%%]QUO1G3hO%'OQUO1G3hO%(qQUO1G3hO%*dQUO1G3hO%,VQUO1G3hO%-xQUO1G3hO%/kQUO1G3hO%1^QUO1G3hO%3PQUO1G3hO%4rQUO'#JVO%5{QUO'#JVO%6TQUO,59SOOQQ,5;W,5;WO%8YQUO,5;WO%8dQUO,5;WO%8iQUO,5;WO%8nQUO,5;WO! iQUO1G2zO%:xQUO1G3wO!1sQUO1G3wO%;PQUO1G3zO%<rQVO1G3zOOQQ1G.}1G.}OOQQ1G.|1G.|OOQQ1G0p1G0pO!>gQ#vO1G2zO%<yQUO'#J{O?pQVO'#HwO%>SQUO'#FTOOQQ'#Ik'#IkO%>XQUO'#FRO%>dQUO'#J{O%>lQUO,5;lO%>qQUO1G.qOOQQ1G.q1G.qOOQR1G0P1G0PO%@dQ!dO'#I]O%@iQbO,59}O%BzQ!eO'#DeO%CRQ!dO'#I_O%CWQbO,5@RO%CWQbO,5@ROOQQ1G/h1G/hO%CcQbO1G/hO%EeQUO'#CyO!F]QbO,59cOOQR1G6U1G6UO#9hQUO1G1kO%GQQUO1G1gOCvQUO1G1kO%G}QUO1G5xO%I^Q#vO'#ElO%JUQbO,59cOOQR-E<V-E<VO%JaQUO1G0aO##qQUO1G0aOOQQ-E<_-E<_O#$XQUO1G0aOOQQ'#Ib'#IbO%KRQUO1G0bO!+aQUO'#EaOOQQ'#Ic'#IcO%LgQUO1G0bO%MZQUO'#E]O%NnQUO'#E_O%NsQUO1G0mO& yQVO1G0mOOQQ1G0m1G0mO&!TQVO1G0mO#EpQUO'#EYO&!yQUO'#EYO&#dQUO'#JuO&#lQUO,5:sOOQQ1G2R1G2RO&#qQUO,59iOOQQ1G0n1G0nO&#qQUO,59iO&$kQUO,59iO!AnQVO1G3pO&%VQUO'#JzO&%_QUO'#K]O&&kQUO'#J_O&&yQUO'#J^O&'UQUO,59gOM{QVO'#IyO&'^QUO,5@}O!,|QUO'#IgO&'fQUO,5@fOOQQ1G0}1G0}OOQQ1G3}1G3}OM{QVO1G3}O&'nQUO'#GpO&'vQUO'#KWOOQQ,5=Z,5=ZOOQR1G6]1G6]O&(RQUO,5=]O&(WQUO,5=^O&(]QVO'#EXO&/_QVO'#FgO!AnQVO,5;|OOQQ,5;|,5;|OCvQUO'#InO&/oQUO,5@kOOQR1G1k1G1kOOQR1G1g1G1gOOQR1G1s1G1sOOQR-E<q-E<qO#9WQUO1G6]OAdQUO1G6]O#9WQUO1G6_OOQR1G6_1G6_OOQR1G6`1G6`O#<|QVO1G6`O&/wQUO1G6`O&0}QVO,5=^OOQR1G1m1G1mOOQQ'#Ie'#IeO&1UQUO,5:|O&2sQUO,5:|OOQQ'#If'#IfO&2zQUO,5:}OOQQ,5:},5:}O&2zQUO,5:}O&3nQVO,5;OO&3xQUO,5;OO&3nQVO,5;OOOQQ,5;O,5;OO&5OQVO,5;OO&5VQUO,5;PO&6PQUO,5;QOOQQ1G3|1G3|O&6WQUO1G/^O/fQUO'#FaOOQR1G1n1G1nO&6]QUO1G0WOOQQ1G0{1G0{O&6bQUO1G0{O&6gQUO1G2zO!+aQUO1G1eOOQR1G2m1G2mO&6xQUO1G2mO&6}QUO,5<_O#EpQUO'#FaOCvQUO'#FaO&7UQUO,5<_OM{QVO,5<_O&7^QUO,5<_O&7cQVO1G1wO!(UQVO1G1wOOQR1G1|1G1|O&<]QUO1G2OOOQR1G2S1G2SO&<bQUO1G2TOCvQUO1G2eO&=kQVO1G2TO&=rQUO1G2TOOQR1G2V1G2VOOQR1G2]1G2]O$#[QUO1G2_OOQQ'#Cu'#CuO&=wQUO'#GZO&>lQUO'#GZO&>qQUO'#KTO$#[QUO'#G^OOQQ'#KU'#KUO&?PQUO1G2_O&?UQVO1G1pOCvQUO'#FaOOQR'#Ip'#IpO&?UQVO1G1pO&ATQUO'#F}OOQR'#Ir'#IrO&AYQVO1G2fO&FVQUO'#GbOOQR1G2j1G2jOOQR,5<T,5<TO&F[QVO,5<TOOQR1G2k1G2kO&FcQUO1G2nOOQR1G2q1G2qOOQQ'#J}'#J}O&GlQUO'#J|O&GwQUO,5;rO&HPQUO'#FXO&HdQUO'#FXO&HwQUO'#F^O&H|QUO1G2qO&IXQUO'#JaOFrQUO'#KVO#9WQUO'#KXOOQR'#Gw'#GwOOQR'#Gx'#GxO&IcQUO'#F_O&IhQUO'#FcO&HwQUO'#FUOOQR1G1[1G1[O&JeQUO1G2sO&KkQUO1G1kO&KvQUO'#JcOOQQ,59v,59vO&LRQUO'#JcO&LZQUO'#DaO&LeQUO'#JvO&M^QUO'#DaOFrQUO'#KVOOQQ'#Gz'#GzOOQQ'#G|'#G|OOQQ'#G}'#G}OOQQ'#Je'#JeOOQQ'#IZ'#IZO&MhQUO,59yOOQQ,59y,59yO&NeQUO'#DaO&NrQUO'#HOOOQQ1G/[1G/[O$4zQUO1G/[O$5PQUO1G/[O&NwQUO1G/[OOQQ1G3V1G3VO$4zQUO1G3VO$5PQUO1G3VO'$YQUO1G3VO''kQUO'#HVO''vQUO'#K[O'(OQUO,5=pO'(TQUO'#KZOOQQ1G3W1G3WO'+qQUO1G3WO'+xQUO1G3WOOQQ1G3^1G3^O$4zQUO1G3^O$5PQUO1G3^O',PQUO1G3^O'/bQUO1G3`O'0hQUO,5@zO!,|QUO1G3qOM{QVO'#HsOOQQ1G3w1G3wO'0mQUO1G3wO'0tQVO1G3zPOOO1G.h1G.hP!6g{&jO1G.hP'2hQUO'#KfP'2s{&jO1G.hP'2x{,UO1G.hPO{O-E<y-E<yPOOO,5AO,5AOP'3Q{&jO,5AOPOOO1G4[1G4[OM{QVO7+$[O'3VQUO'#CyOOQQ,59^,59^O'3bQbO,59cOOQQ,59],59]OOQQ7+)R7+)RO!AnQVO'#IzO'3mQUO,5?qOOQQ1G.n1G.nOOQQ1G0r1G0rO'3uQUO1G0rO'3zQUO7+(fOOQQ7+)c7+)cO'6]QUO7+)cO'6dQUO7+)cO'0tQVO7+)fO! iQUO7+(fO'6kQVO'#IlO'7OQUO,5@gO'7WQUO,5;nOOQQ'#Co'#CoOOQQ,5;o,5;oOM{QVO'#FSOOQQ-E<i-E<iO!AnQVO,5;mOOQQ1G1W1G1WOOQQ,5>w,5>wOOQQ-E<Z-E<ZOOQR'#Dk'#DkOOQR'#Dm'#DmOOQR'#Dp'#DpO'8aQ!eO'#JhO'8hQ7_O'#JhO'8oQ!eO'#JhOOQR'#Jh'#JhOOQR'#I^'#I^O'8vQ!eO,5:POOQQ,5:P,5:PO'8}QbO,5>yOOQQ-E<]-E<]O'9]QbO1G5mOOQQ7+%S7+%SOOQR7+'V7+'VOOQR7+'R7+'RO&KkQUO7+'VO'9hQUO7+%{O##qQUO7+%{OOQQ-E<`-E<`O':YQUO7+%|O';kQUO,5:{O!1sQUO,5:{OOQQ-E<a-E<aO'<kQVO,5:wO'<rQUO,5:yOOQQ7+&X7+&XO'<yQUO7+&XO'>PQVO7+&XO'>xQUO,5:tO'@aQUO'#EbO'ASQUO,5:tO#CyQVO'#EdO'AZQUO'#EeO'BsQUO'#EfO'CZQUO,5:tOM{QVO,5;dO'CeQUO'#EzOOQQ,5;e,5;eO'CvQUO'#IhO'DQQUO,5@aOOQQ1G0_1G0_O'DYQUO1G/TO'ESQUO1G/TO'EnQUO7+)[OOQQ7+)_7+)_OOQQ,5=w,5=wO#/rQVO'#IxO'GaQUO,5?xOOQQ1G/R1G/RO'GlQUO,5?eOOQQ-E<w-E<wOOQQ,5?R,5?ROOQQ-E<e-E<eO'HuQUO7+)iO'HzQUO,5=[O#9OQUO'#IuO'IYQUO,5@rOOQR1G2w1G2wOOQR1G2x1G2xO'IeQUO'#J{O?pQVO'#HwO'JqQUO1G1hOOQQ1G1h1G1hO'KzQUO,5?YOOQQ,5?Y,5?YOOQQ-E<l-E<lOOQR7++w7++wO#9WQUO7++wOOQR7++y7++yO'L`QUO7++zOOQR7++z7++zOOQQ-E<c-E<cO'NTQUO1G0hOOQQ-E<d-E<dOOQQ1G0i1G0iO'N[QUO1G0iO( OQUO1G0jO(!UQVO1G0jOOQQ1G0j1G0jO(!`QVO1G0jOOQQ1G0k1G0kOOQQ7+$x7+$xOOQQ7+%r7+%rOOQQ7+&g7+&gO(!gQUO7+'POOQR7+(X7+(XOOQR1G1y1G1yOCvQUO,5;{O(!}QUO,5<`O(#UQUO1G1yO($_QUO1G1yO!(UQVO7+'cO($dQVO7+'cOM{QVO'#F{O()^QUO7+'jO()cQVO7+'oO()jQUO7+(PO()tQUO7+'oO(*zQVO7+'oO(+RQUO7+'yO(+WQUO,5<uO&=wQUO,5<uO(+xQUO,5<uO&=wQUO,5<uOOQQ,5<v,5<vO(,ZQVO,5<wO$#[QUO'#IqO(,eQUO,5@oO(,mQUO,5<xOOQR7+'y7+'yO(,xQVO7+'[O()mQUO'#KPOOQR-E<n-E<nO#LuQVO,5<iOOQR-E<p-E<pO!,mQUO,5<|OOQR1G1o1G1oO(.wQUO7+(YO(.|QVO7+(YO$)}QUO'#ImO(/UQUO,5@hOOQQ1G1^1G1^O(/aQUO,5;sO!1sQUO,5;vO(/zQUO,5;wO(0YQUO,5;sO(2]QUO,5;xOOQR7+(]7+(]O#IWQUO,5;yO(2eQUO'#JbO(2oQUO'#JbOOQR,5;},5;}O(2|QUO,5;}O!1_QUO,5;}O!0SQUO,5;}O!0hQUO,5;}OOQQ7+(_7+(_O(3RQUO'#IYO(3^QUO,5?}O(3^QUO,5?}O(3fQUO,5?}O(3qQUO,59{O4XQUO,59{OOQQ,5=g,5=gO(3{QUO'#ESO(4|QUO'#ESO(5dQUO,59{O(5nQUO'#E}O(5nQUO'#FOOOQQ'#Jt'#JtO(6YQUO'#JtOM{QVO'#FPOOQQ,59{,59{O(6wQUO,59{O!AnQVO,59{O(6|QUO,5@qOOQQ-E<X-E<XOOQQ1G/e1G/eOOQQ,5=j,5=jOOQQ7+$v7+$vO$4zQUO7+$vO$5PQUO7+$vOOQQ7+(q7+(qO$4zQUO7+(qO$5PQUO7+(qOM{QVO,5=qO(7TQUO'#IwO(7`QUO,5@vOOQQ1G3[1G3[O(7hQUO,5@uOOQQ,5@u,5@uOOQQ7+(r7+(rO(:|QUO7+(rOOQQ7+(x7+(xO$4zQUO7+(xO$5PQUO7+(xOOQQ7+(z7+(zO(?lQVO1G6fO(AnQUO7+)]O(AsQUO,5>_O(ByQUO7+)fPOOO7+$S7+$SP(DlQUO'#KgP(DtQUO,5AQP(Dy{&jO7+$SPOOO1G6j1G6jO(EOQUO<<GvOOQQ1G.x1G.xO(FqQUO,5?fOOQQ,5?f,5?fOOQQ-E<x-E<xOOQQ7+&^7+&^OOQQ<<L}<<L}O(GzQUO<<L}O(I}QUO<<MQO(KpQUO<<LQO(LRQUO,5?WOOQQ,5?W,5?WOOQQ-E<j-E<jOOQQ1G1Y1G1YO'7ZQUO,5;nO(M[QUO1G1XOOQQ1G1X1G1XOOQR,5@S,5@SO(NeQ!eO,5@SO(NlQ7_O,5@SO(NsQ!eO,5@SOOQR-E<[-E<[OOQQ1G/k1G/kO(NzQ!eO'#DzOOQQ1G4e1G4eOOQR<<Jq<<JqO) RQUO<<IgOOQQ'#Ec'#EcO)!}QUO1G0gO)#XQUO1G0gO)#lQUO1G0cOOQQ1G0c1G0cO)$rQUO'#JwO)$zQUO1G0eOOQQ<<Is<<IsO)%bQUO1G0`O)%iQUO'#EfO)%yQUO1G0`O)&QQUO'#IXOOQQ,5;g,5;gOM{QVO1G1OO)'bQUO1G1OOOQQ,5;f,5;fOOQQ,5?S,5?SOOQQ-E<f-E<fO)(qQUO7+$oO))]QUO'#K_O)*iQUO'#K^OOQQ,5?d,5?dOOQQ-E<v-E<vOOQQ<<MT<<MTOOQQ1G2v1G2vOOQQ,5?a,5?aOOQQ-E<s-E<sO'6kQVO,5<[OOQR<= c<= cOOQR<= f<= fOOQQ7+&T7+&TOOQQ7+&U7+&UO)*wQUO7+&UO)+}QVO7+&UOOQR<<Jk<<JkO!AnQVO1G1zO),UQUO1G1zOOQR7+'e7+'eOOQR<<J}<<J}O!(UQVO<<J}OOQR<<KU<<KUO),aQUO<<KZO)-gQVO<<KZO!AnQVO<<KkO)-nQUO<<KZO!(UQVO<<KZO).wQUO<<KZOOQR<<Ke<<KeO).|QUO1G2aO(+WQUO1G2aO&=wQUO1G2aO)/_QUO1G2cO)0eQVO1G2cOOQQ1G2c1G2cO)0oQVO1G2cO)0vQUO,5?]OOQQ-E<o-E<oOOQQ1G2d1G2dOOQR1G2h1G2hOOQR<<Kt<<KtO)1UQVO<<KtO)1aQUO<<KtOOQQ,5?X,5?XOOQQ-E<k-E<kO!1sQUO1G1bOOQQ1G1b1G1bOOQQ1G1c1G1cO)1fQUO'#FXOOQQ1G1d1G1dOOQR1G1i1G1iO)1zQUO1G1iO)2RQUO1G1iO)2YQUO1G1iO)2aQUO,5>tO&LRQUO,5>tOOQQ-E<W-E<WO)2lQUO1G5iO)2lQUO1G5iO)2tQUO1G/gOOQQ1G/g1G/gO)3OQUO1G/gO!AnQVO1G/gO)3TQUO1G/gO)3_QUO,5:nO(3{QUO,5:nO)3uQUO,5:nO(3{QUO,5:nO)5RQUO,5:rO)5oQVO,5;hO(5nQUO'#IjO)5yQUO1G/gO)6[QVO1G/gOOQQ1G3P1G3PO)6cQUO,5;iO)6nQUO,5;jO)7YQUO,5;kO)8`QUO1G/gOOQQ1G3R1G3ROOQQ<<Hb<<HbO$4zQUO<<HbOOQQ<<L]<<L]O$4zQUO<<L]O)9fQUO1G3]OOQQ,5?c,5?cOOQQ-E<u-E<uOOQQ<<L^<<L^OOQQ<<Ld<<LdO$4zQUO<<LdOOQQ<<Lw<<LwO):oQUO1G3yP)<rQUO'#I|P)<zQUO,5ARPO{O1G6l1G6lPOOO<<Gn<<GnOOQQANBiANBiOOQR1G5n1G5nO)=SQ!eO,5:fOOQQ,5:f,5:fO)=xQUO7+&ROOQQ7+%}7+%}O!+aQUO'#IdO)>oQUO,5@cOOQQ7+&P7+&PO)>wQUO7+&jOOQQ,5=x,5=xO)@WQUO1G1vOOQQ<<Ip<<IpO)AdQUO7+'fOOQRAN@iAN@iO)BpQVOAN@uO)BwQUOAN@uO!(UQVOAN@uO)DQQUOAN@uO)DVQUOANAVO)E]QUOANAVOOQRAN@uAN@uO)EbQUO7+'{O(+WQUO7+'{OOQQ7+'}7+'}O)EsQUO7+'}O)FyQVO7+'}O)GQQUOANA`OOQQ7+&|7+&|O)GVQUO,5;sOOQR7+'T7+'TOOQQ1G4`1G4`O)GeQUO1G4`O)GpQUO7++TO)GxQUO7+%ROOQQ7+%R7+%RO)HZQUO7+%RO)6[QVO7+%ROOQQ7+(k7+(kO)H`QUO7+%RO)IfQUO7+%RO!AnQVO7+%RO)IpQUO1G0YO)3_QUO1G0YO(3{QUO1G0YOOQQ'#Ii'#IiO)J[QUO1G0^O)JxQUO1G1SO)LOQVO1G1SOOQQ1G1S1G1SO)LYQVO1G1SO)LaQUO,5?UOOQQ-E<h-E<hOOQQ1G1T1G1TOOQQAN=|AN=|OOQQANAwANAwOOQQANBOANBOOOQQ7+)e7+)ePOQQ,5?h,5?hPOQQ-E<z-E<zOOQQ1G0Q1G0QOOQQ,5?O,5?OOOQQ-E<b-E<bO)LxQUOG26aO!(UQVOG26aO)NRQUOG26aOOQRG26aG26aO!(UQVOG26qO)NWQUO<<KgOOQQ<<Ki<<KiOOQRG26zG26zOOQQ7+)z7+)zOOQQ<<Hm<<HmO)NiQUO<<HmO!AnQVO<<HmO)NnQUO<<HmO* tQUO<<HmO)6[QVO<<HmOOQQ<<LV<<LVO*!VQUO7+%tO)3_QUO7+%tOOQQ-E<g-E<gOOQQ7+&n7+&nO*!qQUO7+&nO*#wQVO7+&nO!(UQVOLD+{OOQRLD+{LD+{OOQRLD,]LD,]OOQQAN>XAN>XO*$OQUOAN>XO*%UQUOAN>XO!AnQVOAN>XO*%ZQUO<<I`OOQQ<<JY<<JYOOQR!$( g!$( gOOQQG23sG23sO*%uQUOG23sO*&{QUOG23sOOQQLD)_LD)_O*'QQUO'#CgO'nQUO'#CgO**}QUO'#CyO*-_QUO'#CyO*.WQUO'#CyO*.hQUO'#CyO*0oQUO'#CyO*0|QUO'#CyO*2oQUO'#CyO*4fQUO'#CyO*4qQbO,59cO*4|QbO,59cO*5XQbO,59cO*5dQUO'#CgO*7hQUO'#CgO*7rQUO'#CgO*:TQUO'#CgO*<zQUO'#CgO*=[QUO'#CgO*>XQUO'#CgO*A_QUO'#CgO*AlQUO'#CgO*AvQbO'#CwO*BXQbO'#CwO*BjQbO'#CwO*B{QUO,5:uO*CcQUO,5:uO*CcQUO,5:uO*C|QbO'#CwO*DXQbO'#CwO*DdQbO'#CwO*DoQbO'#CwO*CcQUO'#EZO*DzQUO'#EZOCvQUO'#EiO*FRQUO'#EiO#3oQUO'#JzO*FsQbO'#CwO*GOQbO'#CwO*GZQUO'#CvO*G`QUO'#CvO*HYQUO'#EbO*IeQUO'#EfO*JqQUO'#CoO*KPQbO,59cO*K[QbO,59cO*KgQbO,59cO*KrQbO,59cO*K}QbO,59cO*LYQbO,59cO*LeQbO,59cO*B{QUO1G0aO*LpQUO1G0aO*CcQUO1G0aO*DzQUO1G0aO*MWQUO,5:|O*NQQUO,5:|O*NwQUO,5;QO+#OQUO'#JaO+#`QUO'#CyO+#nQbO,59cO*B{QUO7+%{O*LpQUO7+%{O+#yQUO,5:{O+$ZQUO'#EbO+$kQUO1G0hO+%|QUO1G0gO+&WQUO1G0gO+&|QUO'#EfO+'mQUO7+&RO+'tQUO'#EZO+'yQUO'#CwO+(OQUO'#EjO+(TQUO'#EjO+(YQUO'#CvO+(_QUO'#CvO+(dQUO'#CwO+(iQUO'#CwO+(nQUO'#CvO+(yQUO'#CvO+)UQUO'#CvO*LpQUO,5:uO*DzQUO,5:uO*DzQUO,5:uO+)aQUO'#JaO+)}QUO'#JaO+*XQUO'#JaO+*lQbO'#CwO+*wQUO'#CrO!+aQUO'#EaO!1sQUO,5:{O+*|QUO'#EZ",
     stateData: "++r~O'tOSSOSTOSRPQVPQ&oPQ&qPQ&rPQ&sPQ&tPQ&uPQ&vPQ&wPQ~O)[OS~OPsO]dOa!ZOdjOlTOr![Os![Ot![Ou![Ov![Ow![Oy!wO{!]O!S}O!ZiO!]!UO!^!TO!l!YO!ouO!p!^O!q!_O!r!_O!s!_O!u!`O!x!aO#S!qO#f#OO#g#PO#j!bO#y!tO#|!{O#}!zO$S!cO$Y!vO$_!nO$`!oO$f!dO$k!eO$m!fO$n!gO$r!hO$t!iO$v!jO$x!kO$z!lO$|!mO%T!pO%Y!rO%]!sO%b!uO%j!xO%u!yO%w!OO%}!|O&O!QO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'xRO(YYO(]aO(_fO(`eO(aoO(bXO)T!VO)U!WO~OR#VOV#QO&o#RO&q#SO&r#TO&s#TO&t#UO&u#UO&v#SO&w#SO~OX#XO'v#XO'w#ZO~O]ZX]iXdiXlgXpZXpiXriXsiXtiXuiXviXwiX{iX!QZX!SiX!ZZX!ZiX!]ZX!^ZX!`ZX!bZX!cZX!eZX!fZX!gZX!iZX!jZX!kZX!piX!qiX!riX!siX!uiX!xiX#jiX'xiX'{ZX'|$bX'}ZX(OZX(WZX(]ZX(]iX(^ZX(_ZX(_iX(`ZX(`iX(aZX(mZX~O(aiX!YZX~P'nO]#pO!Q#^O!Z#aO!]#nO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O'}#`O(O#`O(W#oO(]#bO(^#cO(_#cO(`#dO(a#_O~Od#tO#a#uO&f#vO&i#wO(P#qO~Ol#xO~O!S#yO](TXd(TXr(TXs(TXt(TXu(TXv(TXw(TX{(TX!Z(TX!p(TX!q(TX!r(TX!s(TX!u(TX!x(TX#j(TX'x(TX(](TX(_(TX(`(TX(a(TX~Ol#xO~P-UOl#xO!k#{O(m#{O~OX#|O(c#|O~O!W#}O(W(ZP(e(ZP~Oa!QOl$ROr![Os![Ot![Ou![Ov![Ow![Oy!wO{!]O!p!_O!q!_O!r!_O!s!_O!u!`O#|!{O#}!zO$Y$YO%j!xO%u!yO%w!OO%}!|O&O!QO'x$QO(YYO~O]'hXa'SXd'hXl'SXl'hXr'SXr'hXs'SXs'hXt'SXt'hXu'SXu'hXv'SXv'hXw'SXw'hXy'SX{'SX!Z'hX!o'hX!p'SX!p'hX!q'SX!q'hX!r'SX!r'hX!s'SX!s'hX!u'SX!u'hX!x'hX#j'hX#|'SX#}'SX%b'hX%j'SX%u'SX%w'SX%}'SX&O'SX'x'SX'x'hX(]'hX(_'hX(`'hX~Oa!QOl$ROr![Os![Ot![Ou![Ov![Ow![Oy!wO{!]O!p!_O!q!_O!r!_O!s!_O!u!`O#|!{O#}!zO%j!xO%u!yO%w!OO%}!|O&O!QO'x$QO~Or![Os![Ot![Ou![Ov![Ow![O{!]O!p!_O!q!_O!r!_O!s!_O!u!`O](fXd(fXl(fX!Z(fX!x(fX#j(fX'x(fX(](fX(_(fX(`(fX~O(a$^O~P5rOPsO]dOdjOr![Os![Ot![Ou![Ov![Ow![O!ZiO!]!UO!^!TO!l!YO!x!aO#f#OO#g#PO#j$lO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO(]aO(_fO(`eO(bXO)T!VO)U!WO~Oa$jOl$aO!y$kO'x$_O~P7aO(]$mO~O]$pO!Z$oO~Oa!ZOl8XOy!wO#|!{O#}!zO%j!xO%u!yO%w!OO%}!|O&O!QO'x8OO~P7aOPsO]dOdjO!ZiO!]!UO!^!TO!l!YO!x!aO#f#OO#g#PO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO(]aO(_fO(`eO(bXO)T!VO)U!WO~Oa$jOl$aO#j$lO'x$_O~P:uO]${OdjOl$yO!Z$}O!x!aO#j$lO'x$_O(]$zO(_fO(`fO~Op%QO]'zX](jX!Q'zX!Z'zX!Z(jX!]'zX!^'zX!`'zX!b'zX!c'zX!e'zX!f'zX!g'zX!i'zX!j'zX'{'zX'}'zX(O'zX(W'zX(]'zX(^'zX(_'zX(`'zX(a'zX|'zX|(jX!Y'zX~O!k#{O(m#{O~P=bO!k'zX(m'zX~P=bOPsO]%VOa$jOl$aO!Z%YO![%]O!]!UO!^!TO!l!YO#f#OO#g#PO#j$lO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x$_O(]%WO(`%XO(bXO(m%ZO)T!VO)U!WO~O!S}O'|%^O(m%aO](jX!Z(jX~O]'zX!Q'zX!Z'zX!]'zX!^'zX!`'zX!b'zX!c'zX!e'zX!f'zX!g'zX!i'zX!j'zX'{'zX'}'zX(O'zX(W'zX(]'zX(^'zX(_'zX(`'zX(a'zX!k'zX(m'zX|'zX!Y'zX~O](jX!Z(jX|(jX~PAuO]${OdjOl8_O!Z$}O!x!aO#j$lO'x8PO(]8cO(_8eO(`8eO~O'|%eO~OP%fO'uQO!['zX'|'zXQ'zX!h'zX~PAuO]${OdjOr![Os![Ot![Ou![Ov![Ow![O!Z$}O!p!_O!q!_O!r!_O!s!_O!u!`O!x!aO#j!bO%b!uO(]$zO(_fO(`fO~Ol%hO!o%mO'x$_O~PETO]${OdjOl%hO!Z$}O!x!aO#j!bO'x$_O(]$zO(_fO(`fO~O!S}O(a%qO(m%rO~O!Y%uO~P!QOa%wO%w!OO]%vXd%vXl%vXr%vXs%vXt%vXu%vXv%vXw%vX{%vX!Z%vX!p%vX!q%vX!r%vX!s%vX!u%vX!x%vX#j%vX'x%vX(]%vX(_%vX(`%vX(a%vX|%vX!Q%vX!S%vX!]%vX!^%vX!`%vX!b%vX!c%vX!e%vX!f%vX!g%vX!i%vX!j%vX'{%vX'}%vX(O%vX(W%vX(^%vX!k%vX(m%vXQ%vX!h%vX![%vX'|%vX!Y%vX}%vX#Q%vX#S%vX~Op%QOl(TX|(TXQ(TX!Q(TX!h(TX(W(TX(m(TX~P-UO!k#{O(m#{O]'zX!Q'zX!Z'zX!]'zX!^'zX!`'zX!b'zX!c'zX!e'zX!f'zX!g'zX!i'zX!j'zX'{'zX'}'zX(O'zX(W'zX(]'zX(^'zX(_'zX(`'zX(a'zX|'zX!['zX'|'zX!Y'zXQ'zX!h'zX~OPsO]%VOa$jOl$aO!Z%YO!]!UO!^!TO!l!YO#f#OO#g#PO#j$lO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x$_O(]%WO(`%WO(bXO)T!VO)U!WO~O]&QO!Z&PO(]%|O(_&RO(`&RO~O!S}O~P! iO](TXd(TXl(TXr(TXs(TXt(TXu(TXv(TXw(TX{(TX!Z(TX!p(TX!q(TX!r(TX!s(TX!u(TX!x(TX#j(TX'x(TX(](TX(_(TX(`(TX(a(TX|(TXQ(TX!Q(TX!h(TX(W(TX(m(TX~O]#pO~P!!RO]&VO~O'uQO](gXa(gXd(gXl(gXr(gXs(gXt(gXu(gXv(gXw(gXy(gX{(gX!Z(gX!o(gX!p(gX!q(gX!r(gX!s(gX!u(gX!x(gX#j(gX#|(gX#}(gX%b(gX%j(gX%u(gX%w(gX%}(gX&O(gX'x(gX(](gX(_(gX(`(gX~O]&XO~O]#pO~O]&^O!Z&_O!]&[O!k&[O#b&[O#c&[O#d&[O#e&[O#f&`O#g&`O(O&]O(m&[O~P4XOl8`O%Y&dO'x8QO~O]&eOw&gO~O]&eO~OPsO]%VOa$jOl$aO!S}O!Z%YO!]!UO!^!TO!l!YO#S!qO#f#OO#g#PO#j$lO$_!nO$`!oO$f!dO$k!eO$m!fO$n!gO$r!hO$t!iO$v!jO$x!kO$z!lO%T!pO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x7qO(]%WO(`%WO(aoO(bXO)T!VO)U!WO~O]&kO~O!S#yO(a&mO~PM{O(a&oO~O(a&pO~O'x&qO~Oa!QOl$ROr![Os![Ot![Ou![Ov![Ow![Oy!wO#|!{O#}!zO%j!xO%u!yO%w!OO%}!|O&O!QO'x$QO~O'|&vO~O!S}O~O(a&yO~PM{O!S&{O'x&zO~O]'OO~O]${Oa!QOdjOr![Os![Ot![Ou![Ov![Ow![Oy!wO{!]O!Z$}O!p!_O!q!_O!r!_O!s!_O!u!`O!x!aO#|!{O#}!zO%j!xO%u!yO%w!OO%}!|O&O!QO(]$zO(_fO(`fO~Ol8bOp'RO#j$lO'x8RO~P!-WO]'UOd%aXl%aX!Z%aX!x%aX#j%aX'x%aX(]%aX(_%aX(`%aX~Ol$RO{!]O}'_O!S'ZO'x$QO'|'YO~Ol$RO{!]O}'dO!S'ZO'x$QO'|'YO~Ol$ROy'iO!S'fO#}'iO'x$QO~Ol$RO{!]O}'mO!S'ZO'x$QO'|'YO~Oa!QOl$ROy!wO#|!{O#}!zO%j!xO%u!yO%w!OO%}!|O&O!QO'x$QO~O]'pO~OPsOa$jOl$aO!Z%YO!]!UO!^!TO!l!YO#f#OO#g#PO#j$lO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x$_O(]%WO(`%WO(bXO)T!VO)U!WO~O]'rO(W'tO~P!2mO]#pO~P!1sOPsO]%VOa$jOl$aO!Z'xO!]!UO!^!TO!l!YO#f#OO#g#PO#j$lO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x$_O(]%WO(`%WO(bXO)T!VO)U!WO~OY'yO'uQO'x&zO~O&p'|O~OS(QOT'}O)X(PO~O]#pO't(TO~Q&xXX#XO'v#XO'w(VO~Od(`Ol([O'x(ZO~O!Q&]a!^&]a!`&]a!b&]a!c&]a!e&]a!f&]a!g&]a!i&]a!j&]a'{&]a(W&]a(]&]a(^&]a(_&]a(`&]a(a&]a!k&]a(m&]a|&]a![&]a'|&]a!Y&]aQ&]a!h&]a~OPsOa$jOl$aO!l!YO#f#OO#g#PO#j$lO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x$_O(bXO)T!VO)U!WO]&]a!Z&]a!]&]a'}&]a(O&]a~P!7dO!S#yO|'yP~PM{O]nX]#_XdnXlmXpnXp#_XrnXsnXtnXunXvnXwnX{nX!Q#_X!SnX!ZnX!Z#_X!]#_X!^#_X!`#_X!b#_X!c#_X!e#_X!f#_X!g#_X!i#_X!j#_X!kmX!pnX!qnX!rnX!snX!unX!xnX#jnX'xnX'{#_X'}#_X(O#_X(W#_X(]nX(]#_X(^#_X(_nX(_#_X(`nX(`#_X(mmX|nX|#_X~O(anX(a#_X!Y#_X~P!:zO](qO!Z(rO!](oO!k(oO#b(oO#c(oO#d(oO#e(oO#f(sO#g(sO(O(pO(m(oO~P4XOPsO]%VOa$jOl$aO!]!UO!^!TO!l!YO#f#OO#g#PO#j$lO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x$_O(]%WO(`%WO(bXO)T!VO)U!WO~O!Z(xO~P!?aOd({O#a(|O(P#qO~O!S#yO!Z)OO'})PO!Y(oP~P!?aO!S#yO~PM{O(d)WO~Ol)XO]!VX!Q!VX(W!VX(e!VX~O])ZO!Q)[O(W(ZX(e(ZX~O(W)`O(e)_O~O]iXdiXlgXpiXriXsiXtiXuiXviXwiX{iX!ZiX!piX!qiX!riX!siX!uiX!xiX#jiX'xiX(]iX(_iX(`iX!SiX!QiX(WiX(miX|iX~O(aiX}iX'|iX!]iX!^iX!`iX!biX!ciX!eiX!fiX!giX!iiX!jiX'{iX'}iX(OiX(^iX!kiX![iXQiX!hiX!YiX#QiX#SiX~P!BsO(P)aO~Ol)bO~O](TXd(TXr(TXs(TXt(TXu(TXv(TXw(TX{(TX!Z(TX!p(TX!q(TX!r(TX!s(TX!u(TX!x(TX#j(TX'x(TX(](TX(_(TX(`(TX(a(TX!Q(TX!S(TX!](TX!^(TX!`(TX!b(TX!c(TX!e(TX!f(TX!g(TX!i(TX!j(TX'{(TX'}(TX(O(TX(W(TX(^(TX!k(TX(m(TX|(TX![(TX'|(TXQ(TX!h(TX!Y(TX}(TX#Q(TX#S(TX~Ol)bO~P!FgO(a)cO~P5rOp%QOl(TX~P!FgOr![Os![Ot![Ou![Ov![Ow![O{!]O!p!_O!q!_O!r!_O!s!_O!u!`O](fad(fal(fa!Z(fa!x(fa#j(fa'x(fa(](fa(_(fa(`(fa|(fa!Q(fa(W(fa(m(faQ(fa!h(fa!S(fa'|(fa(a(fa~O]ZXlgXpZXpiX!QZX!SiX!ZZX!]ZX!^ZX!`ZX!bZX!cZX!eZX!fZX!gZX!iZX!jZX!kZX'{ZX'}ZX(OZX(WZX(]ZX(^ZX(_ZX(`ZX(aZX(mZX|ZX~O![ZX'|ZX!YZXQZX!hZX~P!LbO]#pO!Z#aO!]#nO'}#`O(O#`O~O!Q&Sa!^&Sa!`&Sa!b&Sa!c&Sa!e&Sa!f&Sa!g&Sa!i&Sa!j&Sa!k&Sa'{&Sa(W&Sa(]&Sa(^&Sa(_&Sa(`&Sa(a&Sa(m&Sa|&Sa![&Sa'|&Sa!Y&SaQ&Sa!h&Sa~P!NrOd#tO#a)hO&f#vO&i#wO(P7sO~Ol)iO~Ol)iO!S#yO~Ol)iO!k#{O(m#{O~Or![Os![Ot![Ou![Ov![Ow![O~P<pOr![Os![Ot![Ou![Ov![Ow![O!y$kO~P<pO]$pO!Z$oO|!}a!Q!}a!S!}a(a!}a(m!}a'|!}aQ!}a!h!}a~Op%QO!S#yO~O]&^O!Z&_O!]&[O!k&[O#b&[O#c&[O#d&[O#e&[O#f&`O#g&`O(O&]O(m&[O~Or![Os![Ot![Ou![Ov![Ow![O})rO!u!`O#Q)tO#S)uO(O)qO~O]#Oa!S#Oa!Z#Oa'|#Oa(m#Oa|#Oa!Q#Oa(a#OaQ#Oa!h#Oa~P#&dOPsO]%VOa$jOl$aOr![Os![Ot![Ou![Ov![Ow![O!Z%YO!]!UO!^!TO!l!YO#f#OO#g#PO#j$lO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x$_O(`%WO(bXO)T!VO)U!WO~O![)xO(])yO~P#'{O(W)|O(YYO|(iP~P4XO|*OO!Q#^O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO!S#yO](TXr(TXs(TXt(TXu(TXv(TXw(TX|(TX!Z(TX(](TX(_(TX(`(TX~Ol8oO~P#+jO]$pO|*QO!Z$oO~O|*OO~Or![Os![Ot![Ou![Ov![Ow![O|qXQqX!QqX!hqX(WqX(aqX~P! iO|*TO~O]$pO!Z$oO|#]a!Q#]a!S#]a(a#]a(m#]a'|#]aQ#]a!h#]a~Od#tO#a)hO(P7sO~Op%QO](jX!Z(jX!Q(jX!S(jX(a(jX(m(jX|(jX'|(jXQ(jX!h(jX~Op%QO~OPsO]%VOa!ZOl8YOr![Os![Ot![Ou![Ov![Ow![Oy!wO!Z%YO!]!UO!^!TO!l!YO#f#OO#g#PO#j$lO#|!{O#}!zO%j!xO%u!yO%w!OO%}!|O&O!QO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x8SO(]%WO(`%WO(bXO)T!VO)U!WO~OQ(QP!h(QP~P#/rO!Q*ZO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO![)VX~P!NrOp%QO!Q*]O!k#{O(m#{O]'zX!Z'zX!['zX![(nX!]'zX!^'zX!`'zX!b'zX!c'zX!e'zX!f'zX!g'zX!i'zX!j'zX'{'zX'}'zX(O'zX(W'zX(]'zX(^'zX(_'zX(`'zX~O!Q'zX~P#3oOp%QO!['zX'|'zXQ'zX!h'zX~PAuO![*_O~OPsO]%VOa!ZOl8XOr![Os![Ot![Ou![Ov![Ow![Oy!wO!Z%YO!]!UO!^!TO!l!YO#f#OO#g#PO#j$lO#|!{O#}!zO%j!xO%u!yO%w!OO%}!|O&O!QO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x8OO(]%WO(`%WO(bXO)T!VO)U!WO~O!Q*aO![*`O~PM{O!Q*aO![*`O~Ol9lO'x8nO~O!S}O(m%aO~O#g*gO$`*fO~O]*hO!Q*lO!S*iO!Z$oO(m*jO(a(sX~O!Q*lO(a(sX~O(a*oO~OP%fO'uQO]%^a!Q%^a!Z%^a!]%^a!^%^a!`%^a!b%^a!c%^a!e%^a!f%^a!g%^a!i%^a!j%^a'{%^a'}%^a(O%^a(W%^a(]%^a(^%^a(_%^a(`%^a(a%^a!k%^a(m%^a|%^a![%^a'|%^a!Y%^aQ%^a!h%^a~Od#tO#a#uO(P7sO~Ol7{O~O!S}O(a*vO(m*wO~OPsO]%VOa$jOl$aO!Z%YO!]!UO!^!TO!l!YO#f#OO#g*yO#j$lO$`*fO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x$_O(]%WO(`%WO(bXO)T!VO)U!WO~O!Y*zO~P!QO]#pO!Z#aO'}#`O(O#`O!Q&Ya!]&Ya!^&Ya!`&Ya!b&Ya!c&Ya!e&Ya!f&Ya!g&Ya!i&Ya!j&Ya'{&Ya(W&Ya(]&Ya(^&Ya(_&Ya(`&Ya(a&Ya!k&Ya(m&Ya|&Ya![&Ya'|&Ya!Y&YaQ&Ya!h&Ya~Op%QO~PK{O]#pO!Z#aO!]#nO'}#`O(O#`O~P!7dOr![Os![Ot![Ou![Ov![Ow![O|#UX!S#UX(a#UX(m#UXQ#UX!Q#UX!h#UX(W#UX~P! iOr![Os![Ot![Ou![Ov![Ow![O#Q)tO#S)uO(O9nO~O]#VX!S#VX!Z#VX|#VX(a#VX(m#VX!Q#VXQ#VX!h#VX(W#VX~P#B`O]$pO!Z+SO~O![+VO(]+WO~P#'{O]&QO!Z&PO(W)|O(YYO(]%|O(_&RO(`&RO|(iP~P4XO|#YX!S#YX(a#YX(m#YXQ#YX!Q#YX!h#YX(W#YX~P! iO!S}O](lX!Z(lX~O!S&{O$Y!vO(YYO~P4XO|(fX!Q(fX(W(fX(m(fXQ(fX!h(fX!S(fX'|(fX(a(fX~P5rO]#ia!Q#ia!]#ia!^#ia!`#ia!b#ia!c#ia!e#ia!f#ia!g#ia!i#ia!j#ia'{#ia'}#ia(O#ia(W#ia(^#ia(_#ia(`#ia(a#ia!k#ia(m#ia|#ia![#ia!S#ia'|#ia!Y#iaQ#ia!h#ia~O(]+`O!Z#ia~P#FhO|+`O~O![+`O~O!Z+aO(]#ia~P#FhOp%QO(m+cO~O(a+dO~Ol8`O'x8QO~OPsO]%VOa!ZOl8ZOr![Os![Ot![Ou![Ov![Ow![Oy!wO{!]O!Z%YO!]!UO!^!TO!l!YO!p!_O!q!_O!r!_O!s!_O!u!`O#f#OO#g#PO#j$lO#|!{O#}!zO%j!xO%u!yO%w!OO%}!|O&O!QO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO(YYO(]%WO(`%WO(aoO(bXO)T!VO)U!WO~O'x8UO~P#IjO$n+oO~OPsO]%VOa!ZOl8ZOr![Os![Ot![Ou![Ov![Ow![Oy!wO{!]O!Z%YO!]!UO!^!TO!l!YO!p!_O!q!_O!r!_O!s!_O!u!`O#f#OO#g#PO#j$lO#|!{O#}!zO%j!xO%u!yO%w!OO%}!|O&O!QO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x8OO(YYO(]%WO(`%WO(a+sO(bXO)T!VO)U!WO~O!Q#^O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a+uO~P!NrO(a+uO~O(a+vO~O]+|O!x!aO'x+xO(]+yO~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O'|,PO(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO!S}O$f!dO$k!eO$m!fO$n!gO$r,TO$t!iO$v!jO$x!kO$z!lO$|!mO'x7rOd$^a!o$^a!x$^a#S$^a#y$^a$S$^a$Y$^a$_$^a$`$^a%T$^a%Y$^a%]$^a%b$^a'q$^a(_$^a!Y$^a$i$^a~P#IjO%V,WO~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a,XO~P!NrO!Y,YO~P!QO!S&{O~O](TXd(TXr(TXs(TXt(TXu(TXv(TXw(TX{(TX!Z(TX!p(TX!q(TX!r(TX!s(TX!u(TX!x(TX#j(TX'x(TX(](TX(_(TX(`(TX~Ol8rO~P$(WO]$pO!Z$oO(a,^O~Oa!QOl$ROr![Os![Ot![Ou![Ov![Ow![Oy!wO{!]O!p!_O!q!_O!r!_O!s!_O!u!`O#y,dO#|,bO#},cO%j!xO%u!yO%w!OO%}!|O&O!QO'x$QO(YYO~OQ(pP!h(pP~P$)}Ol8hO!o,gO#j!bO#y,mO$S,kO$W,lO$Y!vO%b!uO'x8WO(YYO~P!-WO](UXd(UXr(UXs(UXt(UXu(UXv(UXw(UX{(UX}(UX!S(UX!Z(UX!p(UX!q(UX!r(UX!s(UX!u(UX!x(UX#j(UX'x(UX'|(UX(](UX(_(UX(`(UX(a(UX|(UX!Q(UX(W(UX!](UX!^(UX!`(UX!b(UX!c(UX!e(UX!f(UX!g(UX!i(UX!j(UX'{(UX'}(UX(O(UX(^(UX!k(UX(m(UXQ(UX!h(UX![(UX!Y(UX#Q(UX#S(UX~Ol)bO~P$,lOp%QOl(UX~P$,lOl$RO!P,sO'x$QO~Ol8iO!P-QO!Y-OO!o-PO#j!bO#y,mO$S!cO$W,lO$|!mO%]!sO%b!uO'x8VO(YYO~P!-WO}-TO!S'ZO'|'YO]xadxalxarxasxatxauxavxawxa{xa!Zxa!pxa!qxa!rxa!sxa!uxa!xxa#jxa'xxa(]xa(_xa(`xa~O(axa|xa!Qxa!]xa!^xa!`xa!bxa!cxa!exa!fxa!gxa!ixa!jxa'{xa'}xa(Oxa(Wxa(^xa!kxa(mxaQxa!hxa![xa!Yxa#Qxa#Sxa~P$1eO!S'ZO~O!S'ZO'|'YO~Ol$RO}-TO!S'ZO'x$QO'|'YO~O}-XO!S'ZO'|'YO]%sad%sal%sar%sas%sat%sau%sav%saw%sa{%sa!Z%sa!p%sa!q%sa!r%sa!s%sa!u%sa!x%sa#j%sa'x%sa(]%sa(_%sa(`%sa~O(a%sa|%sa!Q%sa!]%sa!^%sa!`%sa!b%sa!c%sa!e%sa!f%sa!g%sa!i%sa!j%sa'{%sa'}%sa(O%sa(W%sa(^%sa!k%sa(m%saQ%sa!h%sa![%sa!Y%sa#Q%sa#S%sa~P$5jOl$RO}-XO!S'ZO'x$QO'|'YO~O'x&zO!Y)OP~O!S'fO'|-^O]%tad%tal%tar%tas%tat%tau%tav%taw%ta{%ta!Z%ta!p%ta!q%ta!r%ta!s%ta!u%ta!x%ta#j%ta'x%ta(]%ta(_%ta(`%ta(a%ta|%ta!Q%ta!]%ta!^%ta!`%ta!b%ta!c%ta!e%ta!f%ta!g%ta!i%ta!j%ta'{%ta'}%ta(O%ta(W%ta(^%ta!k%ta(m%taQ%ta!h%ta![%ta!Y%ta}%ta#Q%ta#S%ta~Ol$RO!S'fO'x$QO~O}-dO!S'ZO'|'YO]%zad%zal%zar%zas%zat%zau%zav%zaw%za{%za!Z%za!p%za!q%za!r%za!s%za!u%za!x%za#j%za'x%za(]%za(_%za(`%za~O(a%za|%za!Q%za!]%za!^%za!`%za!b%za!c%za!e%za!f%za!g%za!i%za!j%za'{%za'}%za(O%za(W%za(^%za!k%za(m%zaQ%za!h%za![%za!Y%za#Q%za#S%za~P$=WOl$RO}-dO!S'ZO'x$QO'|'YO~O!^#eO!`#fO!j#mO(]#bO!Q&_a!b&_a!c&_a!e&_a!f&_a!g&_a!i&_a'{&_a(W&_a(^&_a(_&_a(`&_a(a&_a!k&_a(m&_a|&_a![&_a'|&_a!Y&_aQ&_a!h&_a~P!NrO]-hO~O]#pO!S#yO!Z-iO!Q&ea!]&ea!^&ea!`&ea!b&ea!c&ea!e&ea!f&ea!g&ea!i&ea!j&ea'{&ea'}&ea(O&ea(W&ea(]&ea(^&ea(_&ea(`&ea(a&ea!k&ea(m&ea|&ea![&ea'|&ea!Y&eaQ&ea!h&ea~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Q&ha(a&ha!k&ha(m&ha|&ha![&ha'|&ha!Y&haQ&ha!h&ha~P!NrOPsO]%VOa$jOl$aO!Z%YO!]!UO!^!TO!l!YO#f#OO#g#PO#j$lO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x$_O(]%WO(`%XO(bXO(m%ZO)T!VO)U!WO~O![-lO~P$FlOS(QOT'}O]#pO)X(PO~O]-oO&p-pO)X-mO~OS-tOT'}O)X-sO~O]#pO~Q&xa!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O'|-vO(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO!Q#^O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a$di|$di~P!NrO]cXlgXpcXpiX!QcX!ZcX!]cX!^cX!`cX!bcX!ccX!ecX!fcX!gcX!icX!jcX!kcX'{cX'}cX(OcX(WcX(]cX(^cX(_cX(`cX(acX(mcX|cX![cX'|cX!ScX!YcXQcX!hcX~Od-xO#a)hO(P-wO~Ol-yO~Op%QO]bi!Qbi!Zbi!]bi!^bi!`bi!bbi!cbi!ebi!fbi!gbi!ibi!jbi!kbi'{bi'}bi(Obi(Wbi(]bi(^bi(_bi(`bi(abi(mbi|bi![bi'|bi!YbiQbi!hbi~O![-{O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO!`#fO(]#bO!Q&Ui!^&Ui!b&Ui!c&Ui!e&Ui!f&Ui!g&Ui!i&Ui!j&Ui'{&Ui(W&Ui(^&Ui(_&Ui(`&Ui(a&Ui!k&Ui(m&Ui|&Ui![&Ui'|&Ui!Y&UiQ&Ui!h&Ui~P!NrO!Q&Ui!^&Ui!`&Ui!b&Ui!c&Ui!e&Ui!f&Ui!g&Ui!i&Ui!j&Ui'{&Ui(W&Ui(]&Ui(^&Ui(_&Ui(`&Ui(a&Ui!k&Ui(m&Ui|&Ui![&Ui'|&Ui!Y&UiQ&Ui!h&Ui~P!NrO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO(]#bO(`#dO!Q&Ui'{&Ui(W&Ui(^&Ui(_&Ui(a&Ui!k&Ui(m&Ui|&Ui![&Ui'|&Ui!Y&UiQ&Ui!h&Ui~P!NrO!^#eO!`#fO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO(]#bO(`#dO!Q&Ui!b&Ui'{&Ui(W&Ui(^&Ui(_&Ui(a&Ui!k&Ui(m&Ui|&Ui![&Ui'|&Ui!Y&UiQ&Ui!h&Ui~P!NrO!^#eO!`#fO!e#kO!f#lO!g#lO!i#lO!j#mO(]#bO(`#dO!Q&Ui!b&Ui!c&Ui'{&Ui(W&Ui(^&Ui(_&Ui(a&Ui!k&Ui(m&Ui|&Ui![&Ui'|&Ui!Y&UiQ&Ui!h&Ui~P!NrO!^#eO!`#fO!e#kO!f#lO!g#lO!i#lO!j#mO(]#bO!Q&Ui!b&Ui!c&Ui'{&Ui(W&Ui(^&Ui(_&Ui(`&Ui(a&Ui!k&Ui(m&Ui|&Ui![&Ui'|&Ui!Y&UiQ&Ui!h&Ui~P!NrO!^#eO!`#fO!f#lO!g#lO!i#lO!j#mO(]#bO!Q&Ui!b&Ui!c&Ui!e&Ui'{&Ui(W&Ui(^&Ui(_&Ui(`&Ui(a&Ui!k&Ui(m&Ui|&Ui![&Ui'|&Ui!Y&UiQ&Ui!h&Ui~P!NrO!^#eO!`#fO!j#mO(]#bO!Q&Ui!b&Ui!c&Ui!e&Ui!f&Ui!g&Ui!i&Ui'{&Ui(W&Ui(^&Ui(_&Ui(`&Ui(a&Ui!k&Ui(m&Ui|&Ui![&Ui'|&Ui!Y&UiQ&Ui!h&Ui~P!NrO!^#eO!`#fO(]#bO!Q&Ui!b&Ui!c&Ui!e&Ui!f&Ui!g&Ui!i&Ui!j&Ui'{&Ui(W&Ui(^&Ui(_&Ui(`&Ui(a&Ui!k&Ui(m&Ui|&Ui![&Ui'|&Ui!Y&UiQ&Ui!h&Ui~P!NrO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO(]#bO(^#cO(_#cO(`#dO!Q&Ui'{&Ui(W&Ui(a&Ui!k&Ui(m&Ui|&Ui![&Ui'|&Ui!Y&UiQ&Ui!h&Ui~P!NrO!Q-|O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO|'yX~P!NrO!Q-|O|'yX~O|.OO~O]#`ap#`a!Q#`a!]#`a!^#`a!`#`a!b#`a!c#`a!e#`a!f#`a!g#`a!i#`a!j#`a'{#`a'}#`a(O#`a(W#`a(^#`a(_#`a(`#`a(a#`a!k#`a(m#`a|#`a![#`a!S#`a'|#`a!Y#`aQ#`a!h#`a~O(].PO!Z#`a~P%6YO|.PO~O![.PO~O!Z.QO(]#`a~P%6YO]#pO!S#yO!Q&ei!]&ei!^&ei!`&ei!b&ei!c&ei!e&ei!f&ei!g&ei!i&ei!j&ei'{&ei'}&ei(O&ei(W&ei(]&ei(^&ei(_&ei(`&ei(a&ei!k&ei(m&ei|&ei![&ei'|&ei!Y&eiQ&ei!h&ei~O!Z-iO~P%8xO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Q&hi(a&hi!k&hi(m&hi|&hi![&hi'|&hi!Y&hiQ&hi!h&hi~P!NrO![.VO~P$FlO!Q.XO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Y(oX~P!NrO'x.[O~O!Z.^O'})PO(m.`O~O!Q.XO!Y(oX~O!Y.aO~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO(W#oO(]#bO(^#cO(_#cO(`#dO!Q_i'{_i(a_i!k_i(m_i|_i![_i'|_i!Y_iQ_i!h_i~P!NrO!W.bO~Ol)XO]!Va!Q!Va(W!Va(e!Va~OP.jO].iOl.jO!Q.jO!S.gO!W.jO!Z.hO!].jO!^.jO!`.jO!b.jO!c.jO!e.jO!f.jO!g.jO!i.jO!j.jO!k.jO!l.jO'uQO'|.jO'}.jO(O.jO(W.jO(].dO(^.eO(_.eO(`.fO(a.jO(bXO~O|.mO~P%@zO!W#}O~O!Q)[O(W(Za(e(Za~O(e.qO~O]nXdnXlmXpnXrnXsnXtnXunXvnXwnX{nX!ZnX!pnX!qnX!rnX!snX!unX!xnX#jnX'xnX(]nX(_nX(`nX!SnX!QnX(WnX|nXQnX!hnX~O(anX}nX'|nX!]nX!^nX!`nX!bnX!cnX!enX!fnX!gnX!inX!jnX'{nX'}nX(OnX(^nX!knX(mnX![nX!YnX#QnX#SnX~P%ChO(a.sO~Or![Os![Ot![Ou![Ov![Ow![O{!]O!p!_O!q!_O!r!_O!s!_O!u!`O~O](fid(fil(fi!Z(fi!x(fi#j(fi'x(fi(](fi(_(fi(`(fi|(fi!Q(fi(W(fi(m(fiQ(fi!h(fi!S(fi'|(fi(a(fi~P%GVO](qO!Z(rO!](oO!k(oO#b(oO#c(oO#d(oO#e(oO#f(sO#g(sO(O(pO(m(oO~Od({O#a)hO(P7sO~O]$pO!Z$oO|!}i!Q!}i!S!}i(a!}i(m!}i'|!}iQ!}i!h!}i~O]#Oi!S#Oi!Z#Oi'|#Oi(m#Oi|#Oi!Q#Oi(a#OiQ#Oi!h#Oi~P#&dOr![Os![Ot![Ou![Ov![Ow![O})rO#Q)tO#S)uO(O)qO~O]#Oi!S#Oi!Z#Oi'|#Oi(m#Oi|#Oi!Q#Oi(a#OiQ#Oi!h#Oi~P%KuO].|Or#PXs#PXt#PXu#PXv#PXw#PX}#PX!S#PX!Z#PX#Q#PX#S#PX'|#PX(O#PX(m#PX|#PX!Q#PX(a#PXQ#PX!h#PX(W#PX~O].}O~O![/OO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO![/OO(]/QO~P#'{O![/OO~PM{O]/VOdjOl8_O!Z/UO!x!aO#j$lO'x$_O(W/ZO(]/SO~O(_/WO(`/WO(m/YO|!|X!Q!|XQ!|X!h!|X~P&![O!Q/]O|(iX~O|/_O~Or![Os![Ot![Ou![Ov![Ow![O|qaQqa!Qqa!hqa(Wqa(aqa~P! iO]$pO!Z+SO|qaQqa!Qqa!hqa(Wqa(aqa~O!Q*]O![(nX~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dOQ)PX!Q)PX!h)PX~P!NrO(W/dOQ(RX!Q(RX!h(RX~O!Q/eOQ(QX!h(QX~OQ/gO!h/gO~O!Q*ZO![)Va~O!Q*]O![(na~O]#pO!S#yO~O!Q/nO!S(zX(m(zX~O(a/pO~O(a/qO~OPsO]%VOa!ZOl8YOy!wO!S#yO!Z%YO!]!UO!^!TO!l!YO#f#OO#g#PO#j$lO#|!{O#}!zO%j!xO%u!yO%w!OO%}!|O&O!QO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x8TO(W)|O(YYO(]%WO(`%WO(bXO)T!VO)U!WO|'yP|(iP~P%GVOPsO]dOa!ZOdjOlTOr![Os![Ot![Ou![Ov![Ow![Oy!wO{!]O!]!UO!^!TO!l!YO!ouO!p!^O!q!_O!r!_O!s!_O!u!`O!x!aO#S!qO#f#OO#g#PO#j!bO#y!tO#|!{O#}!zO$S!cO$Y!vO$_!nO$`!oO$f!dO$k!eO$m!fO$n!gO$r!hO$t!iO$v!jO$x!kO$z!lO$|!mO%T!pO%Y!rO%]!sO%b!uO%j!xO%u!yO%w!OO%}!|O&O!QO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'xRO(YYO(]aO(_fO(`eO(aoO(bXO)T!VO)U!WO~O!S*iO!Y%uO!Z/sO'})PO~P&*tO!Q*lO(a(sa~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a/}O~P!NrO(a/qO~P!4iOr![Os![Ot![Ou![Ov![Ow![O|#Ua!S#Ua(a#Ua(m#Ua!Q#UaQ#Ua!h#Ua(W#Ua~P! iO]$pO!Z+SO|#Ua!S#Ua(a#Ua(m#Ua!Q#UaQ#Ua!h#Ua~O(W#Ua~P&2UO]#Va!S#Va!Z#Va|#Va(a#Va(m#Va!Q#VaQ#Va!h#Va(W#Va~P#B`O![0VO(]0WO~P#'{O![0VO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO![0VO~PM{O]$pO|0XO!Z+SO~O]$pO!Z+SO|#Ya!S#Ya(a#Ya(m#Ya!Q#YaQ#Ya!h#Ya~O(W#Ya~P&5bO|0YO~O|0ZO~O![0[O~O]$pO!Z+SO!S%hi(a%hi(m%hi~O(a0^O~O|0_O~P+WO|0_O(a#_O~O|0_O~O$i0dOP$ei]$eia$eid$eil$eir$eis$eit$eiu$eiv$eiw$eiy$ei{$ei!S$ei!Z$ei!]$ei!^$ei!l$ei!o$ei!p$ei!q$ei!r$ei!s$ei!u$ei!x$ei#S$ei#f$ei#g$ei#j$ei#y$ei#|$ei#}$ei$S$ei$Y$ei$_$ei$`$ei$f$ei$k$ei$m$ei$n$ei$r$ei$t$ei$v$ei$x$ei$z$ei$|$ei%T$ei%Y$ei%]$ei%b$ei%j$ei%u$ei%w$ei%}$ei&O$ei&Z$ei&[$ei&`$ei&d$ei&m$ei&n$ei'q$ei'u$ei'x$ei(Y$ei(]$ei(_$ei(`$ei(a$ei(b$ei)T$ei)U$ei!Y$ei~O]0fO~O!Q#^O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a0hO~P!NrO(a0kO~PM{O(a0hO~O]+|Or![Os![Ot![Ou![Ov![Ow![O!x!aO!y$kO'x+xO(]+yO~O(]0pO~O]$pO!Q0sO!Z0rO(a(wX~O(a0vO~O!S}O$f!dO$k!eO$m!fO$n!gO$r,TO$t!iO$v!jO$x!kO$z!lO$|!mO'x7rOd$^i!o$^i!x$^i#S$^i#y$^i$S$^i$Y$^i$_$^i$`$^i%T$^i%Y$^i%]$^i%b$^i'q$^i(_$^i!Y$^i$i$^i~P#IjO]0zO~O%V,WOP%Si]%Sia%Sid%Sil%Sir%Sis%Sit%Siu%Siv%Siw%Siy%Si{%Si!S%Si!Z%Si!]%Si!^%Si!l%Si!o%Si!p%Si!q%Si!r%Si!s%Si!u%Si!x%Si#S%Si#f%Si#g%Si#j%Si#y%Si#|%Si#}%Si$S%Si$Y%Si$_%Si$`%Si$f%Si$k%Si$m%Si$n%Si$r%Si$t%Si$v%Si$x%Si$z%Si$|%Si%T%Si%Y%Si%]%Si%b%Si%j%Si%u%Si%w%Si%}%Si&O%Si&Z%Si&[%Si&`%Si&d%Si&m%Si&n%Si'q%Si'u%Si'x%Si(Y%Si(]%Si(_%Si(`%Si(a%Si(b%Si)T%Si)U%Si!Y%Si$i%Si~O]$pO~O!Y0}O~P!QO|1OO!Q1PO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO!Q1QOQ(pX!h(pX~OQ1SO!h1SO~O(W1VO(m1UOQ#{X!Q#{X!h#{X~P!1sO(W1VO(m1UOQ#{X!Q#{X!h#{X~P!1_Op'RO~O]$pO!Z$oO(a1YO~Ol7}O(a(TX~P$(WO'x+xO~Oa!QOl$ROy1aO#|!{O#}1`O$Y!vO%j1bO%u!yO%w!OO%}!|O&O!QO'x$QO(YYO~P%GVO|1cO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO]$pO!S}O!Z$oO~O!Q1dO(W1fO!S(VX~Ol$RO'x$QO~O!o1iO(YYO~P4XO(a1jO~PAdO]1nOl([O!S#yO!x!aO#j$lO'x(ZO'|1rO(]1kO(_1oO(`1oO~O(a1sO(m1uO~P&LlOl8iO!P-QO!Y1xO!o-PO#j!bO#y,mO$S!cO$W,lO$|!mO%]!sO%b!uO'x8VO(YYO~P!-WOl8iO#j!bO'x8VO~P!-WO'|1yO~O}1|O!S'ZO'|'YO]xidxilxirxisxitxiuxivxiwxi{xi!Zxi!pxi!qxi!rxi!sxi!uxi!xxi#jxi'xxi(]xi(_xi(`xi(axi|xi!Qxi!]xi!^xi!`xi!bxi!cxi!exi!fxi!gxi!ixi!jxi'{xi'}xi(Oxi(Wxi(^xi!kxi(mxiQxi!hxi![xi!Yxi#Qxi#Sxi~O}2PO!S'ZO'|'YO]%sid%sil%sir%sis%sit%siu%siv%siw%si{%si!Z%si!p%si!q%si!r%si!s%si!u%si!x%si#j%si'x%si(]%si(_%si(`%si(a%si|%si!Q%si!]%si!^%si!`%si!b%si!c%si!e%si!f%si!g%si!i%si!j%si'{%si'}%si(O%si(W%si(^%si!k%si(m%siQ%si!h%si![%si!Y%si#Q%si#S%si~O(m2QO!Q%yX!Y%yX~O!Q2RO!Y)OX~O!Y2TO~Oa2VOl$RO%w!OO'x$QO~O!S'fO]%tid%til%tir%tis%tit%tiu%tiv%tiw%ti{%ti!Z%ti!p%ti!q%ti!r%ti!s%ti!u%ti!x%ti#j%ti'x%ti(]%ti(_%ti(`%ti(a%ti|%ti!Q%ti!]%ti!^%ti!`%ti!b%ti!c%ti!e%ti!f%ti!g%ti!i%ti!j%ti'{%ti'}%ti(O%ti(W%ti(^%ti!k%ti(m%tiQ%ti!h%ti![%ti!Y%ti}%ti#Q%ti#S%ti~O'|%ti~P'(cO'|-^O~P'(cO}2[O!S'ZO'|'YO]%zid%zil%zir%zis%zit%ziu%ziv%ziw%zi{%zi!Z%zi!p%zi!q%zi!r%zi!s%zi!u%zi!x%zi#j%zi'x%zi(]%zi(_%zi(`%zi(a%zi|%zi!Q%zi!]%zi!^%zi!`%zi!b%zi!c%zi!e%zi!f%zi!g%zi!i%zi!j%zi'{%zi'}%zi(O%zi(W%zi(^%zi!k%zi(m%ziQ%zi!h%zi![%zi!Y%zi#Q%zi#S%zi~O|2]O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO|2^O~O!Z&ei~P%8xOPsOa$jOl$aO!]!UO!^!TO!l!YO#f#OO#g#PO#j$lO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x$_O(bXO)T!VO)U!WO]&ka!S&ka!Z&ka(]&ka(_&ka(`&ka~O'x&zO(W2cO|)ZP~O)X2bO~O&p2eO)X2bO~O)X2fO~OlmXpnXp&RX~Od2hO#a)hO(P-wO~O!Q-|O|'ya~O![2lO~O]$pO!Z+SO!S%hq(a%hq(m%hq~O]#pO!S#yO!Q&eq!]&eq!^&eq!`&eq!b&eq!c&eq!e&eq!f&eq!g&eq!i&eq!j&eq'{&eq'}&eq(O&eq(W&eq(]&eq(^&eq(_&eq(`&eq(a&eq!k&eq(m&eq|&eq![&eq'|&eq!Y&eqQ&eq!h&eq~O!Z&eq~P'4]O!Z-iO~P'4]O!S#yO!Z)OO'})PO!Q'`X!Y'`X~P!?aO!Q.XO!Y(oa~O!Q*ZO![2tO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO!Y2xO~P%@zO![2xO~P%@zO|2xO~P%@zO|2}O~P%@zO]3OO!Q'Ra(W'Ra(e'Ra~O!Q)[O(W(Zi(e(Zi~O]$pO!Z$oO|!}q!Q!}q!S!}q(a!}q(m!}q'|!}qQ!}q!h!}q~O]#Oq!S#Oq!Z#Oq'|#Oq(m#Oq|#Oq!Q#Oq(a#OqQ#Oq!h#Oq~P%KuO]&QO!Z&PO!S#Ta(m#Ta|#Ta!Q#Ta(a#TaQ#Ta!h#Ta~O(]8lO(_8mO(`8mOr#Tas#Tat#Tau#Tav#Taw#Ta}#Ta#Q#Ta#S#Ta'|#Ta(O#Ta~P':|O|3WO~PM{O|(kP~P!+aO![3ZO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO![3ZO~PM{O]/VOdjOl$yO!Z/UO!x!aO#j$lO'x$_O(]9TO(_3]O(`3]O~O|!|a!Q!|aQ!|a!h!|a~P'>WO]/VOdjOr![Os![Ot![Ou![Ov![Ow![O!Z/UO!x!aO!y$kO#j$lO'x$_O|#UX!Q#UXQ#UX!h#UX~Ol8_O(]/SO(_9XO(`9XO~P'?YO]$pO|!|a!Q!|aQ!|a!h!|a~O!Z+SO~P'@qO]/VOa!QOdjOl8aOy!wO!Z/UO!x!aO#j$lO#|!{O#}!zO%j!xO%u!yO%w!OO%}!|O&O!QO'x8RO(W)|O(YYO(]9TO(_3]O(`3]O|(iP~P%GVO(_9XO(`9XO|#YX!Q#YXQ#YX!h#YX~P&![O!Z$oO(m3aO~P'@qO'x&zO|#nX!Q#nXQ#nX!h#nX~O(W3dO(YYO~P4XO!Q/]O|(ia~Or![Os![Ot![Ou![Ov![Ow![O|qiQqi!Qqi!hqi(Wqi(aqi~P! iO]$pO!Z+SO|qiQqi!Qqi!hqi(Wqi(aqi~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Q&^q(a&^q!k&^q(m&^q|&^q![&^q'|&^q!Y&^qQ&^q!h&^q~P!NrO!Q/eOQ(Qa!h(Qa~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Q'ma!['ma~P!NrO![3kO~O(W3lO!Q%da!S%da(m%da~O!Q/nO!S(za(m(za~O!Q3oO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a#_O!Y(oX~P!NrO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Q$Ui(a$Ui~P!NrO]*hO!S#yO!Z$oO(m*jO!Q'ba(a'ba~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a3qO~P!NrO]$pO!Z+SO|#Ui!S#Ui(a#Ui(m#Ui!Q#UiQ#Ui!h#Ui~O(W#Ui~P'MfO]#Vi!S#Vi!Z#Vi|#Vi(a#Vi(m#Vi!Q#ViQ#Vi!h#Vi(W#Vi~P#B`O![3sO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO![3sO(]3uO~P#'{O![3sO~PM{O(a3vO~O]*hO!Q*lO!S#yO!Z$oO(a(sX~O(m3wO~P(!lO|3yO!Q#^O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO|3yO~O$i3{OP$eq]$eqa$eqd$eql$eqr$eqs$eqt$equ$eqv$eqw$eqy$eq{$eq!S$eq!Z$eq!]$eq!^$eq!l$eq!o$eq!p$eq!q$eq!r$eq!s$eq!u$eq!x$eq#S$eq#f$eq#g$eq#j$eq#y$eq#|$eq#}$eq$S$eq$Y$eq$_$eq$`$eq$f$eq$k$eq$m$eq$n$eq$r$eq$t$eq$v$eq$x$eq$z$eq$|$eq%T$eq%Y$eq%]$eq%b$eq%j$eq%u$eq%w$eq%}$eq&O$eq&Z$eq&[$eq&`$eq&d$eq&m$eq&n$eq'q$eq'u$eq'x$eq(Y$eq(]$eq(_$eq(`$eq(a$eq(b$eq)T$eq)U$eq!Y$eq~O(a3|O~O(a4OO~PM{O'|4PO(m*jO~P(!lO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a4OO~P!NrO|4RO~PM{O(a4TO~O]+|Or![Os![Ot![Ou![Ov![Ow![O!x!aO'x+xO(]+yO~O]$pO!Z0rO!Q$}a(a$}a|$}a~O![4ZO(]4[O~P#'{O!Q0sO(a(wa~O]$pO|4_O!Z0rO~O!S}O$f!dO$k!eO$m!fO$n!gO$r,TO$t!iO$v!jO$x!kO$z!lO$|!mO'x7rOd$^q!o$^q!x$^q#S$^q#y$^q$S$^q$Y$^q$_$^q$`$^q%T$^q%Y$^q%]$^q%b$^q'q$^q(_$^q!Y$^q$i$^q~P#IjO(a4aO~OP4bO'uQO~O!Q1QOQ(pa!h(pa~Op%QO(m4fOQ#{al(TX!Q#{a!h#{a(W(TX~P$(WO'x+xOQ$Pa!Q$Pa!h$Pa~Op%QO(m4fOQ#{a](UXd(UXl(UXr(UXs(UXt(UXu(UXv(UXw(UX{(UX}(UX!Q#{a!S(UX!Z(UX!h#{a!p(UX!q(UX!r(UX!s(UX!u(UX!x(UX#j(UX'x(UX'|(UX(W(UX(](UX(_(UX(`(UX~O#|4iO#}4iO~Ol)bO(a(UX~P$(WOp%QOl(TX(a(UX~P$(WO(a4kO~Ol$RO!P4pO'x$QO~O!Q1dO!S(Va~O!Q1dO(W4sO!S(Va~O(a4uO(m4wO~P&LlO]1nOl([Or![Os![Ot![Ou![Ov![Ow![O!x!aO!y$kO#j$lO'x(ZO(]1kO(_1oO(`1oO~O(]4|O~O]$pO!Q5PO!S*iO!Z5OO'|1rO~O(a4uO(m5RO~P(5RO]1nOl([O!x!aO#j$lO'x(ZO(]1kO(_1oO(`1oO~Op%QO](hX!Q(hX!S(hX!Z(hX'|(hX(a(hX(m(hX|(hX~O(a4uO~O(a5XO~PAdO'x&zO!Q'kX!Y'kX~O!Q2RO!Y)Oa~Op%QO](}ad(}al(}ar(}as(}at(}au(}av(}aw(}a{(}a!S(}a!Z(}a!p(}a!q(}a!r(}a!s(}a!u(}a!x(}a#j(}a'x(}a(](}a(_(}a(`(}a(a(}a|(}a!Q(}a!](}a!^(}a!`(}a!b(}a!c(}a!e(}a!f(}a!g(}a!i(}a!j(}a'{(}a'}(}a(O(}a(W(}a(^(}a!k(}a(m(}aQ(}a!h(}a![(}a'|(}a!Y(}a}(}a#Q(}a#S(}a~O!S'fO]%tqd%tql%tqr%tqs%tqt%tqu%tqv%tqw%tq{%tq!Z%tq!p%tq!q%tq!r%tq!s%tq!u%tq!x%tq#j%tq'x%tq(]%tq(_%tq(`%tq(a%tq|%tq!Q%tq!]%tq!^%tq!`%tq!b%tq!c%tq!e%tq!f%tq!g%tq!i%tq!j%tq'{%tq'}%tq(O%tq(W%tq(^%tq!k%tq(m%tqQ%tq!h%tq![%tq'|%tq!Y%tq}%tq#Q%tq#S%tq~OPsOa$jOl$aO!S#yO!l!YO#f#OO#g#PO#j$lO&Z!TO&[!TO&`!}O&d!YO&m!YO&n!YO'uQO'x$_O(bXO)T!VO)U!WO~O])Si!Q)Si!Z)Si!])Si!^)Si!`)Si!b)Si!c)Si!e)Si!f)Si!g)Si!i)Si!j)Si'{)Si'})Si(O)Si(W)Si(])Si(^)Si(_)Si(`)Si(a)Si!k)Si(m)Si|)Si![)Si'|)Si!Y)SiQ)Si!h)Si~P(>_O|5dO~O![5eO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Q&hq(a&hq!k&hq(m&hq|&hq![&hq'|&hq!Y&hqQ&hq!h&hq~P!NrO!Q5fO|)ZX~O|5hO~O)X5iO~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Q^y(a^y!k^y(m^y|^y![^y'|^y!Y^yQ^y!h^y~P!NrO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO|'na!Q'na~P!NrO]#pO!S#yO!Q&ey!Z&ey!]&ey!^&ey!`&ey!b&ey!c&ey!e&ey!f&ey!g&ey!i&ey!j&ey'{&ey'}&ey(O&ey(W&ey(]&ey(^&ey(_&ey(`&ey(a&ey!k&ey(m&ey|&ey![&ey'|&ey!Y&eyQ&ey!h&ey~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Q&hy(a&hy!k&hy(m&hy|&hy![&hy'|&hy!Y&hyQ&hy!h&hy~P!NrO]$pO!Z+SO!S%hy(a%hy(m%hy~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Q'`a!Y'`a~P!NrO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Q#ui!Y#ui~P!NrO!Y5kO~P%@zO![5kO~P%@zO|5kO~P%@zO|5mO~P%@zO]$pO!Z$oO|!}y!Q!}y!S!}y(a!}y(m!}y'|!}yQ!}y!h!}y~Or#Tis#Tit#Tiu#Tiv#Tiw#Ti}#Ti!S#Ti#Q#Ti#S#Ti'|#Ti(O#Ti(m#Ti|#Ti!Q#Ti(a#TiQ#Ti!h#Ti~O]$pO!Z+SO~P) sO]&QO!Z&PO(]8lO(_8mO(`8mO~P) sO|5oO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO!Q5pO|(kX~O|5rO~O]$pO|!|i!Q!|iQ!|i!h!|i~O!Z+SO~P)%PO|#YX!Q#YXQ#YX!h#YX~P'>WO!Z$oO~P)%PO]'XXd&{Xl&{Xr'XXs'XXt'XXu'XXv'XXw'XX|'XX!Q'XX!Z'XX!x&{X#j&{X'x&{X(]'XX(_'XX(`'XXQ'XX!h'XX~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO|#li!Q#liQ#li!h#li~P!NrO]$pO!Z+SO|qqQqq!Qqq!hqq(Wqq(aqq~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dOQ)RX!Q)RX!h)RX~P!NrO(W5tOQ)QX!Q)QX!h)QX~O![5vO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO![5vO~PM{O|$hi!Q$Ua(a$Ua~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a5yO~P!NrO|5{O~PM{O|5{O!Q#^O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO|5{O~O]$pO!Z0rO!Q$}i(a$}i|$}i~O![6SO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO![6SO(]6UO~P#'{O![6SO~PM{O]$pO!Z0rO!Q'ea(a'ea~OP%fO|6VO'uQO~O|6VO~O'x+xO(W1VO(m1UOQ#{X!Q#{X!h#{X~O(a6YO~P$=WO(a6YO~P$1eO(a6YO~P$5jO(W6ZO!Q&|a!S&|a~O!Q1dO!S(Vi~O(a6_O(m6aO~P(5RO(a6_O~O(a6_O(m6eO~P&LlOr![Os![Ot![Ou![Ov![Ow![O~P(5nO]$pO!Z5OO!Q!va!S!va'|!va(a!va(m!va|!va~Or![Os![Ot![Ou![Ov![Ow![O}6iO#Q)tO#S)uO(O)qO~O]!za!Q!za!S!za!Z!za'|!za(a!za(m!za|!za~P)4aO![6mO(]6nO~P#'{O!Q5PO!S#yO'|1rO(a6_O(m6eO~O!S#yO~P#<|O]$pO|6qO!Z5OO~O]$pO!Z5OO!Q#ra!S#ra'|#ra(a#ra(m#ra|#ra~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a#sa~P!NrO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a6_O~P!NrO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Q%yi!Y%yi~P!NrO!Z-iO]&gi!Q&gi!S&gi!]&gi!^&gi!`&gi!b&gi!c&gi!e&gi!f&gi!g&gi!i&gi!j&gi'{&gi'}&gi(O&gi(W&gi(]&gi(^&gi(_&gi(`&gi(a&gi!k&gi(m&gi|&gi![&gi'|&gi!Y&giQ&gi!h&gi~O'x&zO(W6vO~O!Q5fO|)Za~O|6xO~P%@zO]$pO!Z+SO!S#Tq(m#Tq|#Tq!Q#Tq(a#TqQ#Tq!h#Tq~Or#Tqs#Tqt#Tqu#Tqv#Tqw#Tq}#Tq#Q#Tq#S#Tq'|#Tq(O#Tq~P)=ZO!Q5pO|(ka~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO|#lq!Q#lqQ#lq!h#lq~P!NrO!Q#^O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO!Y'`a(a$di~P!NrO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO|$hq!Q$Ui(a$Ui~P!NrO|6|O~PM{O|6|O!Q#^O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO|6|O~O|7PO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO|7PO~O]$pO!Z0rO!Q$}q(a$}q|$}q~O![7RO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO![7RO~PM{O(a7SO~O(m4fOQ#{a!Q#{a!h#{a~O(W7TO!Q&|i!S&|i~O!Q1dO!S(Vq~O!Q5PO!S#yO'|1rO(a7UO(m7WO~O(a7UO~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a7UO~P!NrO(a7UO(m7ZO~P(5RO]$pO!Z5OO!Q!vi!S!vi'|!vi(a!vi(m!vi|!vi~O]!zi!Q!zi!S!zi!Z!zi'|!zi(a!zi(m!zi|!zi~P)4aO![7`O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO![7`O(]7bO~P#'{O![7`O~PM{O]$pO!Z5OO!Q'^a!S'^a'|'^a(a'^a(m'^a~O|7cO!Q#^O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO|7cO~O]$pO!Z0rO!Q$}y(a$}y|$}y~O(a7fO~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a7fO~P!NrO!Q5PO!S#yO'|1rO(a7fO(m7iO~O]$pO!Z5OO!Q!vq!S!vq'|!vq(a!vq(m!vq|!vq~O![7kO!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO~P!NrO![7kO~PM{O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a7mO~P!NrO(a7mO~O]$pO!Z5OO!Q!vy!S!vy'|!vy(a!vy(m!vy|!vy~O!^#eO!`#fO!b#hO!c#iO!e#kO!f#lO!g#lO!i#lO!j#mO'{#[O(W#oO(]#bO(^#cO(_#cO(`#dO(a7pO~P!NrO(a7pO~O]ZXlgXpZXpiX!QZX!SiX!ZZX!]ZX!^ZX!`ZX!bZX!cZX!eZX!fZX!gZX!iZX!jZX!kZX'{ZX'|$bX'}ZX(OZX(WZX(]ZX(^ZX(_ZX(`ZX(aZX(mZX~O]#_XlmXpnXp#_X!Q#_X!SnX!Z#_X!]#_X!^#_X!`#_X!b#_X!c#_X!e#_X!f#_X!g#_X!i#_X!j#_X!kmX'{#_X'}#_X(O#_X(W#_X(]#_X(^#_X(_#_X(`#_X(mmX|#_XQ#_X!h#_X~O(a#_X![#_X'|#_X!Y#_X~P*(}O]nX]#_XdnXlmXpnXp#_XrnXsnXtnXunXvnXwnX{nX!ZnX!Z#_X!pnX!qnX!rnX!snX!unX!xnX#jnX'xnX(]nX(_nX(`nX|nX|#_X!QnX(WnX~O(anX(mnX~P*+_O]#_XlmXpnXp#_X!Q#_X!Z#_X|#_XQ#_X!h#_X~O!S#_X(a#_X(m#_X'|#_X~P*-iOQnXQ#_X!QnX!hnX!h#_X(WnX~P!:zO]nX]#_XlmXpnXp#_XrnXsnXtnXunXvnXwnX{nX!SnX!Z#_X!pnX!qnX!rnX!snX!unX!xnX#jnX'xnX(]nX(_nX(`nX~O'|nX(anX(mnX~P*/OOdnX|#_X!Q#_X!ZnX!]#_X!^#_X!`#_X!b#_X!c#_X!e#_X!f#_X!g#_X!i#_X!j#_X!kmX'{#_X'}#_X(O#_X(W#_X(]#_X(^#_X(_#_X(`#_X(a#_X(mmX~P*/OO]nX]#_XdnXlmXpnXp#_XrnXsnXtnXunXvnXwnX{nX!ZnX!Z#_X!pnX!qnX!rnX!snX!unX!xnX#jnX'xnX(]nX(_nX(`nX(a#_X~OlmXpnX(a#_X~Od({O#a(|O(P7sO~Od({O#a(|O(P7wO~Od({O#a(|O(P7tO~O]iXriXsiXtiXuiXviXwiX|iX!ZiX(]iX(_iX(`iXdiX{iX!piX!qiX!riX!siX!uiX!xiX#jiX'xiX~P!LbO]ZXlgXpZXpiX!QZX!ZZX(aZX(mZX~O!SZX'|ZX~P*6|OlgXpiX(aZX(miX~O]ZX]iXdiXlgXpZXpiXriXsiXtiXuiXviXwiX{iX!ZZX!ZiX!piX!qiX!riX!siX!uiX!xiX#jiX'xiX(]iX(_iX(`iX|ZX|iX!QiX(WiX(miX~O(aZX~P*8QO]ZX]iXlgXpZXpiXriXsiXtiXuiXviXwiX!QZX!QiX!SiX!ZZX!ZiX!]ZX!^ZX!`ZX!bZX!cZX!eZX!fZX!gZX!iZX!jZX!kZX'{ZX'}ZX(OZX(WZX(WiX(]ZX(]iX(^ZX(_ZX(_iX(`ZX(`iX(mZX~OQZXQiX!hZX!hiX~P*:[OdiX{iX|ZX|iX!piX!qiX!riX!siX!uiX!xiX#jiX'xiX(miX~P*:[O]iXdiXriXsiXtiXuiXviXwiX{iX!ZiX!piX!qiX!riX!siX!uiX!xiX#jiX'xiX(]iX(_iX(`iX~P!LbO]ZX]iXlgXpZXpiXriXsiXtiXuiXviXwiX{iX!ZZX!piX!qiX!riX!siX!uiX!xiX#jiX'xiX(]iX(_iX(`iX(aiX~O!SiX'|iX(miX~P*?nOdiX!ZiX~P*?nOd#tO#a)hO&f#vO&i#wO(P#qO~Od#tO#a)hO&f#vO&i#wO(P7vO~Od#tO#a)hO&f#vO&i#wO(P7xO~Or![Os![Ot![Ou![Ov![Ow![O~PCvOr![Os![Ot![Ou![Ov![Ow![O!y$kO~PCvOd#tO#a)hO(P7uO~Od#tO#a)hO(P7zO~Od#tO#a)hO(P7tO~Od#tO#a)hO(P7yO~O]${OdjOl8_Or![Os![Ot![Ou![Ov![Ow![O!Z$}O!x!aO!y$kO#j$lO'x$_O(]8dO(_8fO(`8fO~O]${OdjOl8_O!Z$}O!x!aO#j$lO'x$_O(]8dO(_8fO(`8fO~Od#tO#a#uO(P7tO~Od#tO#a#uO(P7wO~Ol7}O~Ol7|O~O]&QOr![Os![Ot![Ou![Ov![Ow![O!Z&PO(]8lO(_8mO(`8mO~O}#UX!S#UX#Q#UX#S#UX'|#UX(O#UX(m#UX|#UX!Q#UX(a#UXQ#UX!h#UX~P*GeO]&QO!Z&PO(]8lO(_8mO(`8mO~Or#YXs#YXt#YXu#YXv#YXw#YX}#YX!S#YX#Q#YX#S#YX'|#YX(O#YX(m#YX|#YX!Q#YX(a#YXQ#YX!h#YX~P*ISO]cXlgXpiX!ScX~Od({O#a)hO(P#qO~Od({O#a)hO(P7uO~Od({O#a)hO(P7zO~Od({O#a)hO(P7yO~Od({O#a)hO(P7tO~Od({O#a)hO(P7vO~Od({O#a)hO(P7xO~Or![Os![Ot![Ou![Ov![Ow![O~P*FRO}#Ua!S#Ua#Q#Ua#S#Ua'|#Ua(O#Ua(m#Ua|#Ua!Q#Ua(a#UaQ#Ua!h#Ua~P*GeOr#Uas#Uat#Uau#Uav#Uaw#Ua}#Ua#Q#Ua#S#Ua'|#Ua(O#Ua~P&2UOr#Yas#Yat#Yau#Yav#Yaw#Ya}#Ya#Q#Ya#S#Ya'|#Ya(O#Ya~P&5bO](TXr(TXs(TXt(TXu(TXv(TXw(TX{(TX!p(TX!q(TX!r(TX!s(TX!u(TX!x(TX#j(TX'x(TX(](TX(_(TX(`(TX(m(TX~Ol7|O!S(TX'|(TX(a(TX~P+ nO]&RXlmXpnX!S&RX~Od2hO#a)hO(P9OO~O(]%|O(_&RO(`&RO(W#Ta~P':|Ol$yO(]9TO(_3]O(`3]O~P'?YOr#Uis#Uit#Uiu#Uiv#Uiw#Ui}#Ui#Q#Ui#S#Ui'|#Ui(O#Ui~P'MfO!S#Ti|#Ti(a#Ti(m#Ti!Q#TiQ#Ti!h#Ti(W#Ti~O]$pO!Z+SO~P+%bO]&QO!Z&PO(]%|O(_&RO(`&RO~P+%bOdjOl8_O!x!aO#j$lO'x$_O~O]/VO!Z/UO(]/SO(_9XO(`9XO|#YX!Q#YXQ#YX!h#YX~P+&kO(W#Tq~P)=ZO(]8^O~Ol8oO~Ol8pO~Ol8qO~Ol8rO~Ol8sO~Ol8tO~Ol8uO~Ol8oO!k#{O(m#{O~Ol8tO!k#{O(m#{O~Ol8uO!k#{O(m#{O~Ol8tO!S#yOQ(TX!Q(TX!h(TX(W(TX|(TX(m(TX~P$(WOl8uO!S#yO~P$(WOl8sO|(TX!Q(TX(W(TX(m(TX~P$(WOd-xO#a)hO(P9OO~Ol9PO~O(]9hO~OV&o&r&s&q'u(b!W'xST#b!^!`&td#c!l&[!j]&p)[&u'}!b!c&v&w&v~",
     goto: "$@Y)[PPPPPP)]P)`PP,r1vP4l4l7dP7d:[P:u;X;mAtHTNh!&_P!,h!-]!.QP!.lPPPPPP!/SP!0gPPP!1vPP!2|P!4f!4j!5]P!5cPPPPP!5fP!5fPP!5fPPPPPPPP!5r!8vPPPPP!8yP:x!:UPP:x!<Z!>c!>p!@T!ArP!ArP!BS!Bh!CV!Bh!Bh!Bh!>p!>p!>p!Cv!HP!HnPPPPPPP!Ie!MhP!>p!>c!>c##z#$Q:x:x:x#$T#$h#&p#&x#&x#'PP#'a#'hPP#'h#'h#'o#'PP#'s#(d#'YP#(oP#)R#*{#+U#+_PP#+t#,_#,{#-i#+tP#.t#/QP#+tP#+tPP#/T#+t#+tP#+tP#+tP#+tP#+tP#1zP#2_#2_#2_#2_#+_#+_P#2lP#+_#*{P#2p#2pP#2}#*{#*{#5xP#6]#6h#6n#6n#*{#7d#*{P#8O#8O!4f!4f!4f!4f!4f!4f!/S!/SP#8RP#9i#9w!/S!/S!/SPP#9}#:Q!I]#:T7d4l#<w#>g#?|4lPP4l#Af4lP4l4l4lP4lP#DY4lP#Af#Df4lPPPPPPPPPPP)]P#GY#G`#Iv#JV#J]#KY#K`#Kv#LQ#MY#NX#N_#Ni#No#N{$ V$ _$ e$ k$ y$!S$![$!b$!m$!|$#W$#^$#d$#k$#z$$Q$%i$%o$%u$%|$&T$&^PPPPPPPP$&d$&hPPPPP$,p#9}$,s$0O$2V$3YP$3]P$3a$3dPPPPPPPPP$3p$5]$6d$7V$7]$9f$9iP$;O$;U$;Y$;]$;c$;o$;y$<S$=R$=n$=t$>_$>o$>r$?S$?a$?g#9}#:Q#:Q$?jPP$?m$?xP$@S$@VR#WP&jsOadei|}!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%Q%V%W%X%Y%e%r%t&P&e&f&j&k&v&{'O'U'p'r'x(x)O)w)y*T*Z*a*h*i*j*w*y+S+U+W+j+m+s,P,S,Z-i-l-v-|.V.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7iU%fs%g4bQ&W!^Q'y#Qd.j)Z.g.h.i.l2y2z2{3O5lR4b1PdgOade|}%t&{*i,Z#^$|fmtu!t$W$f$g$m$z${%m'S'T'V'Z)f)l)n){*l+h+r,Q,g,w,}-P.v/R/S/V/W0`3]8[8]8^8c8d8e8f8v8w8x8y9Q9R9T9X9f9g9hS%Si/s&O%z!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%Q%V%W%X%Y%e%r&P&e&f&j&k&v'O'U'p'r'x(x)O)w)y*T*Z*a*h*j*w*y+S+U+W+j+m+s,P,S-i-l-v-|.V.X.^.`.|/Q/Y/e0U0W0d0f0h0k0r0w0z1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7iQ&c!cQ&}!rQ'y#TQ'z#QQ'{#RQ*U$}Q+[&VQ+e&dS-Z'f2RQ/j*]Q2_-hQ2c-oQ3c/ZQ6v5fR8g/U$f#]S!Z$`$j$q%R%y%{&l&u&x'q'w(W(X(a(b(c(d(e(f(g(h(i(j(k(l(w(})U)v*V*x+T+f+q,],o-f.Z/P/b/h/r/t/|0T0b0j2`2a2g2i2o2q2u2v3V3b3g3t3}4Q4X5V5W5^5s5u5w5z5}6T6c6k6{7X7a7g7nQ&Y!aQ'v#OQ(S#VQ(v#v[*k%b)d/v0a0i0xQ+_&XQ-j'uQ-n'zQ-u(TS.S(u-kQ/m*bS2m.T.UR5j2n&k!YOadei|}!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%Q%V%W%X%Y%e%r%t&P&e&f&j&k&v&{'O'U'p'r'x(x)O)w)y*T*Z*a*h*i*j*w*y+S+U+W+j+m+s,P,S,Z-i-l-v-|.V.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7i&k!SOadei|}!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%Q%V%W%X%Y%e%r%t&P&e&f&j&k&v&{'O'U'p'r'x(x)O)w)y*T*Z*a*h*i*j*w*y+S+U+W+j+m+s,P,S,Z-i-l-v-|.V.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7iQ(^#`S*b%^/nQ.])Pk1q,v1h1k1n1o4x4y4z4|5P6g6h7^Q(`#`k1p,v1h1k1n1o4x4y4z4|5P6g6h7^l(_#`,v1h1k1n1o4x4y4z4|5P6g6h7^T*b%^/n^UO|}%t&{*i,Z#`$S[_!b!m!v!w!x!y!z!{#O#u#v$Y$p$s&Q&W&s'R'Y'`'e'i'n'v(v(|)q)z+]+c+g,b,c,l,s,t-^.z.}/]1Q1U1`1a1b1d1i4f4p5p9n9o&[$baefi!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$f$g$m$o$z${%W%X%Y%e%r&P&f&j'O'S'U'p'x(x)O)l)n)w)y*T*Z*a*j*w*y+S+U+W+j+m+s-i-l-v-|.V.X.^.`.v.|/Q/R/U/Y/s0U0W0d0f0h0k0r1r1u2Q2^3]3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7i9TY%itu%m,g,wl(]#`,v1h1k1n1o4x4y4z4|5P6g6h7^Q8j'TU8k'Z,}-PU9[d%V'r![9]m$W'V)f){*l+h+r,Q/S/W0`8[8]8^8c8d8e8f8v8w8x8y9Q9R9X9f9g9hS9^!c&dQ9_!tQ9`/VU9a%Q*h/e^9b&e&k&v,P,S0w0zT9m%^/n^VO|}%t&{*i,ZQ$S-^!j$T[_!b!m!v!{#O#u#v$Y$p$s&Q&W&s'R'v(v(|)q)z+]+c+g,b,t.z.}/]1Q1U1i4f5p9n9oj$bf$f$g$m$z${'S)l)n.v/R3]9T%p$caei!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%W%X%Y%e%r&P&f&j'O'U'p'x(x)O)w)y*T*Z*a*j*w*y+S+U+W+j+m+s-i-l-v-|.V.X.^.`.|/Q/U/Y/s0U0W0d0f0h0k0r1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7iU$rd%V'rY%itu%m,g,wQ'P!tp'W!w!x!y!z'Y'`'e'i'n,c,s1`1a1b1d4pl(]#`,v1h1k1n1o4x4y4z4|5P6g6h7^Q,f'TQ1[,lU8}'Z,}-P![9]m$W'V)f){*l+h+r,Q/S/W0`8[8]8^8c8d8e8f8v8w8x8y9Q9R9X9f9g9hS9^!c&dU9i%Q*h/e^9j&e&k&v,P,S0w0zQ9k/VT9m%^/nx!ROd|}%Q%V%t&e&k&v&{'r*h*i,P,S,Z/e0w0z!t$X[_!b!m!t!v!{#O#u#v$Y$p$s&Q&W&s'R'T'Z'v(v(|)q)z+]+c+g,t,}-P.z.}/V/]1Q1U1i4f5p9n9o%p$iaei!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%W%X%Y%e%r&P&f&j'O'U'p'x(x)O)w)y*T*Z*a*j*w*y+S+U+W+j+m+s-i-l-v-|.V.X.^.`.|/Q/U/Y/s0U0W0d0f0h0k0r1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7i#t%Ofmtu#`$W$f$g$m$z${%^%m&d'S'V)f)l)n){*l+h+r,Q,g,v,w.v/R/S/W/n0`1h1k1n1o3]4x4y4z4|5P6g6h7^8[8]8^8c8d8e8f8v8w8x8y9Q9R9T9X9f9g9hQ&b!cn'X!w!x!y!z'Y'`'e'i'n,s1`1a1b1d4pf+}&t+w+y+|0m0n0p0s4V4W6RQ1T,bQ1W,cQ1Z,kQ1],lQ2U-^Q4h1VR6X4ix!ROd|}%Q%V%t&e&k&v&{'r*h*i,P,S,Z/e0w0z!v$X[_!b!m!t!v!{#O#u#v$Y$p$s&Q&W&s'R'T'Z'v(v(|)q)z+]+c+g,b,t,}-P.z.}/V/]1Q1U1i4f5p9n9o%p$iaei!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%W%X%Y%e%r&P&f&j'O'U'p'x(x)O)w)y*T*Z*a*j*w*y+S+U+W+j+m+s-i-l-v-|.V.X.^.`.|/Q/U/Y/s0U0W0d0f0h0k0r1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7i#v%Ofmtu!c#`$W$f$g$m$z${%^%m&d'S'V)f)l)n){*l+h+r,Q,g,v,w.v/R/S/W/n0`1h1k1n1o3]4x4y4z4|5P6g6h7^8[8]8^8c8d8e8f8v8w8x8y9Q9R9T9X9f9g9hp'X!w!x!y!z'Y'`'e'i'n,c,s1`1a1b1d4pQ1],lR2U-^^WO|}%t&{*i,Z#`$S[_!b!m!v!w!x!y!z!{#O#u#v$Y$p$s&Q&W&s'R'Y'`'e'i'n'v(v(|)q)z+]+c+g,b,c,l,s,t-^.z.}/]1Q1U1`1a1b1d1i4f4p5p9n9oj$bf$f$g$m$z${'S)l)n.v/R3]9T%p$daei!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%W%X%Y%e%r&P&f&j'O'U'p'x(x)O)w)y*T*Z*a*j*w*y+S+U+W+j+m+s-i-l-v-|.V.X.^.`.|/Q/U/Y/s0U0W0d0f0h0k0r1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7iY%itu%m,g,wl(]#`,v1h1k1n1o4x4y4z4|5P6g6h7^Q8j'TU8k'Z,}-P![9]m$W'V)f){*l+h+r,Q/S/W0`8[8]8^8c8d8e8f8v8w8x8y9Q9R9X9f9g9hS9^!c&dQ9_!tQ9`/VU9cd%V'rU9d%Q*h/e^9e&e&k&v,P,S0w0zT9m%^/np#rT$R$a$y%h([8X8Y8Z8_8`8a8b8h8i9lo(y#x)b)i-y7{7|7}8o8p8q8r8s8t8u9Pp#sT$R$a$y%h([8X8Y8Z8_8`8a8b8h8i9lo(z#x)b)i-y7{7|7}8o8p8q8r8s8t8u9P^%Pgh$|%S%T%z8gd%x!R$X$i%O&b'X1T1W1]2UV-z(^(_1qS$wd%VQ*W%QQ-g'rQ0]+cQ3X.}Q3h/eR6y5p#s!QO[_d|}!b!m!t!v!{#O#u#v$Y$p$s%Q%V%t&Q&W&e&k&s&v&{'R'T'Z'r'v(v(|)q)z*h*i+]+c+g,P,S,Z,b,l,t,}-P.z.}/V/]/e0w0z1Q1U1i4f5p9n9o#O^O[_`|}!b!t!v#u$V$Y$[$]$p%t&Q&W&Z&e&k&v&{'R'T'Z(|)g)z*h*i+]+g,P,S,Z,l,t,}-P/V/]0w0z1Q1iS'`!w1aS'e!x1bV'n!z,c1`S'^!w1aS'c!x1bU'l!z,c1`W-S'['_'`4mW-W'a'd'e4nW-c'j'm'n4lS1{-T-US2O-X-YS2Z-d-eQ5Z1|Q5]2PR5c2[S']!w1aS'b!x1bU'k!z,c1`Y-R'['^'_'`4mY-V'a'c'd'e4nY-b'j'l'm'n4lU1z-S-T-UU1}-W-X-YU2Y-c-d-eS5Y1{1|S5[2O2PS5b2Z2[Q6r5ZQ6s5]R6t5cT,{'Z,}!aZO[|}$p%t&Q&W&e&k&v&{'R'T'Z)z*h*i+]+g,P,S,Z,l,t,}/V/]0w0z1QQ$OYR.n)[R)^$Oe.j)Z.g.h.i.l2y2z2{3O5l&j!YOadei|}!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%Q%V%W%X%Y%e%r%t&P&e&f&j&k&v&{'O'U'p'r'x(x)O)w)y*T*Z*a*h*i*j*w*y+S+U+W+j+m+s,P,S,Z-i-l-v-|.V.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7ie.j)Z.g.h.i.l2y2z2{3O5lR3P.nd]O|}%t&{'T'Z*i,Z,}!j^[_`!b!t!v#u$V$Y$[$]$p&Q&W&Z&e&k&v'R(|)g)z*h+]+g,P,S,l,t-P/V/]0w0z1Q1iQ%ktT)o$n)p!fbOadeftu|}!t$f$g$m$z${%m%t&{'S'T'Z)l)n*i,Z,g,w,}-P.v/R/V3]9Tf+z&t+w+y+|0m0n0p0s4V4W6Rj1l,v1h1k1n1o4x4y4z4|5P6g6h7^r9Zm$W'V)f*l+h+r,Q0`8[8]8^8c8e8v8x9Qi9p){/S/W8d8f8w8y9R9X9f9g9hv$nc$h$t$x%b'Q)d)k,e,p.t.u/X/v0a0i0x3R3^|%}!X$v%|&Q&R&a(t){*P*R*|.W/R/S/V/W/`3]9S9T9W9XY+Q3T5n8{8|9Un+R&O*S*}+X+Y+b.R/T/a0P2p3[3f9V9Y^0q+{0o0u4U4]6Q7QQ0|,WY3S.y3U8l8m8ze4}1m4t4{5T5U6d6f6o7]7jW)|$p&Q*h/VS,_'R1QR3d/]#sjOadefmtu|}!t$W$f$g$m$z${%m%t&{'S'T'V'Z)f)l)n){*i*l+h+r,Q,Z,g,w,}-P.v/R/S/V/W0`3]8[8]8^8c8d8e8f8v8w8x8y9Q9R9T9X9f9g9h#Qjadefm!t$W$f$g$m$z${'S'V)f)l)n){*l+h+r,Q.v/R/S/V/W0`3]8[8]8^8c8d8e8f8v8w8x8y9Q9R9T9X9f9g9h`kO|}%t&{'T*i,ZU%jtu,gQ*s%mS,u'Z,}T1v,w-PW)r$n)p)s.xW+O%}+P+R0ST6i4}6jW)r$n)p)s.xQ+Q%}S0R+P+RQ3r0ST6i4}6j!X&S!X$v%|&Q&R&a(t){*P*R*|.W.y/R/S/V/W/`3U3]8l8m8z9S9T9W9X!U&S$v%|&Q&R&a(t){*P*R*|.W.y/R/S/V/W/`3U3]8l8m8z9S9T9W9XR&T!XdhOade|}%t&{*i,Z#^$|fmtu!t$W$f$g$m$z${%m'S'T'V'Z)f)l)n){*l+h+r,Q,g,w,}-P.v/R/S/V/W0`3]8[8]8^8c8d8e8f8v8w8x8y9Q9R9T9X9f9g9h&U%Ti!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%Q%V%W%X%Y%e%r&P&e&f&j&k&v'O'U'p'r'x(x)O)w)y*T*Z*a*h*j*w*y+S+U+W+j+m+s,P,S-i-l-v-|.V.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7iQ&c!cR+e&dj#tT$a$y%h8X8Y8Z8_8`8a8b8h8ii({#x)i7{7|7}8o8p8q8r8s8t8uj#tT$a$y%h8X8Y8Z8_8`8a8b8h8ih({#x)i7{7|7}8o8p8q8r8s8t8uS-x([9lT2h-y9P#^jfmtu!t$W$f$g$m$z${%m'S'T'V'Z)f)l)n){*l+h+r,Q,g,w,}-P.v/R/S/V/W0`3]8[8]8^8c8d8e8f8v8w8x8y9Q9R9T9X9f9g9hdlOade|}%t&{*i,Z&V!Yi!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%Q%V%W%X%Y%e%r&P&e&f&j&k&v'O'U'p'r'x(x)O)w)y*T*Z*a*h*j*w*y+S+U+W+j+m+s,P,S-i-l-v-|.V.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7i#^jfmtu!t$W$f$g$m$z${%m'S'T'V'Z)f)l)n){*l+h+r,Q,g,w,}-P.v/R/S/V/W0`3]8[8]8^8c8d8e8f8v8w8x8y9Q9R9T9X9f9g9hdlOade|}%t&{*i,Z&U!Yi!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%Q%V%W%X%Y%e%r&P&e&f&j&k&v'O'U'p'r'x(x)O)w)y*T*Z*a*h*j*w*y+S+U+W+j+m+s,P,S-i-l-v-|.V.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7ik1p,v1h1k1n1o4x4y4z4|5P6g6h7^Q/[){R3`/WR/[){Q1t,vS4v1h1mU6`4t4x5QS7V6^6dR7h7Y^#zV!R$c$i$r9i9jQ&n!iS(m#p*hS)S#y*iQ)V#{Y*k%b)d/v0i0xQ-j'uS.S(u-kS/c*T2^Q/m*bS/u*j3wQ1t,vQ2j-|S2m.T.US2r.X3oQ2w.`Q3x0aU4v1h1m1uQ5j2nQ6O4PY6`4t4w4x5Q5RW7V6^6a6d6eU7h7W7Y7ZR7o7iS)S#y*iT2r.X3oZ)Q#y)R*i.X3o^zO|}%t&{*i,ZQ,n'TT,{'Z,}S'T!t,mR1X,dS,_'R1QR4j1XT,_'R1Q^zO|}%t&{*i,ZQ+^&WQ+j&eS+s&k0zW,R&v,P,S0wQ,n'TR1^,l[%cm$W+h+r,Q0`R/w*l^zO|}%t&{*i,ZQ+^&WQ,n'TR1^,l!OqO|}!f%e%t&f&j&v&{*i+m,P,S,Z0d0w3{4R5{6|7P7cS%_k,uS%pw,hQ&U!XQ&w!pU*e%`%j1vQ*n%bS*u%n%oQ+Z&TQ+n&hS.r)d,pS/y*r*sQ/{*tQ3Q.tQ3p/zQ4`0|Q5S1mQ6b4tR7[6d_zO|}%t&{*i,ZQ&|!rQ+^&WR,[&}wrO|}!f%e%t&f&j&{*i+m,Z0d3{4R5{6|7P7c!PqO|}!f%e%t&f&j&v&{*i+m,P,S,Z0d0w3{4R5{6|7P7c!OnO|}!f%e%t&f&j&v&{*i+m,P,S,Z0d0w3{4R5{6|7P7cR&r!l!OqO|}!f%e%t&f&j&v&{*i+m,P,S,Z0d0w3{4R5{6|7P7cR+j&e!OpO|}!f%e%t&f&j&v&{*i+m,P,S,Z0d0w3{4R5{6|7P7cW$ud%V'r0fQ&n!iS(Y#^3oQ+i&eS+t&k0zQ0c+jQ4S0kQ5|4OR6}5yQ&f!dQ&h!eQ&j!gR+m&gR+k&e&b!SOadei|}!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#p#w#y#{$o%Q%V%W%X%Y%e%r%t&P&e&f&j&k&v&{'O'U'p'r'x(x)O)w)y*T*Z*a*h*i*j*w*y+S+U+W+j+m+s,P,S,Z-i-v-|.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7iR0g+o^zO|}%t&{*i,ZW,R&v,P,S0wT,{'Z,}g+}&t+w+y+|0m0n0p0s4V4W6RT,U&w,V^zO|}%t&{*i,ZT,{'Z,}&j!YOadei|}!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#n#p#w#y#{$o%Q%V%W%X%Y%e%r%t&P&e&f&j&k&v&{'O'U'p'r'x(x)O)w)y*T*Z*a*h*i*j*w*y+S+U+W+j+m+s,P,S,Z-i-l-v-|.V.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q2^3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7iR4c1P^uO|}%t&{*i,ZQ%mtQ,g'TT,w'Z,}S%`k,uS*r%j1vR/z*sQ*c%^R3m/nS%_k,uS%pw,hU*e%`%j1vS*u%n%oS/y*r*sQ/{*tQ3p/zQ5S1mQ6b4tR7[6dbwO|}%t&{'Z*i,Z,}S%nt,gU%ou,w-PQ*t%mR,h'TR,n'T#r!QO[_d|}!b!m!t!v!{#O#u#v$Y$p$s%Q%V%t&Q&W&e&k&s&v&{'R'T'Z'r'v(v(|)q)z*h*i+]+c+g,P,S,Z,b,l,t,}-P.z.}/V/]/e0w0z1Q1U1i4f5p9n9oR2V-^Q'h!yS-_'g'iS2W-`-aR5a2XQ-['fR5_2RR*X%QR3i/e&c!SOadei|}!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#p#w#y#{$o%Q%V%W%X%Y%e%r%t&P&e&f&j&k&v&{'O'U'p'r'x(x)O)w)y*T*Z*a*h*i*j*w*y+S+U+W+j+m+s,P,S,Z-i-v-|.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7i$Z#fS$q%R&l&u&x'q'w(W(X(a(b(d(e(f(g(h(i(j(k(l(w(})U)v*V*x+T+f+q,],o-f.Z/P/b/h/r/t/|0T0b0j2`2a2g2i2o2q2u2v3V3b3g3t3}4Q4X5V5W5^5s5u5w5z5}6T6c6k6{7X7a7g7n#w#gS$q%R&l&u&x'w(W(X(a(k(l(w(})U)v*V*x+T+f+q,],o-f.Z/P/b/h/r/t/|0T0b0j2`2a2g2i2o2q2u2v3V3b3g3t3}4Q4X5V5W5^5s5u5w5z5}6T6c6k6{7X7a7g7n#}#jS$q%R&l&u&x'w(W(X(a(d(e(f(k(l(w(})U)v*V*x+T+f+q,],o-f.Z/P/b/h/r/t/|0T0b0j2`2a2g2i2o2q2u2v3V3b3g3t3}4Q4X5V5W5^5s5u5w5z5}6T6c6k6{7X7a7g7n&c!YOadei|}!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#p#w#y#{$o%Q%V%W%X%Y%e%r%t&P&e&f&j&k&v&{'O'U'p'r'x(x)O)w)y*T*Z*a*h*i*j*w*y+S+U+W+j+m+s,P,S,Z-i-v-|.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7iQ-k'uQ.T(uQ2n.UR6u5e&c!XOadei|}!T!U!f!i!n!q!}#P#[#^#a#e#f#g#h#i#j#k#l#m#p#w#y#{$o%Q%V%W%X%Y%e%r%t&P&e&f&j&k&v&{'O'U'p'r'x(x)O)w)y*T*Z*a*h*i*j*w*y+S+U+W+j+m+s,P,S,Z-i-v-|.X.^.`.|/Q/U/Y/e/s0U0W0d0f0h0k0r0w0z1r1u2Q3a3o3u3w3{4O4P4R4Y4[4w5O5R5y5{6U6a6e6l6n6|7P7W7Z7b7c7iQ#YQR(U#YU$fa$z9T`$sd%Q%V'r+c.}/e5pQ&s!m!Q)j$f$s&s)l)w*R+U.v/`0U0m4V4Y4y6R6g6l7^8[8v8w9Q9R9fS)l$g$mQ)w$oQ*R$vS+U&P/UQ.v)nQ/`*PQ0U+SQ0m+yS4V0n0pQ4Y0rQ4y1kQ6R4WS6g4z4|Q6l5OQ7^6hQ8[8cS8v8]8^S8w9g9hQ9Q8xQ9R8yT9f/S8dQ1e,qU4q1e4r6]S4r1f1gR6]4sQ,}'ZR1w,}`[O|}%t&{'T*i,ZY$U[)z+]+g,t^)z$p&Q'R*h/V/]1QS+]&W,l^+g&e&k&v,P,S0w0zT,t'Z,}Q)Y#}R.c)YQ.l)ZQ2y.gQ2z.hQ2{.iY2|.l2y2z2{5lR5l3OQ)]$OS.o)].pR.p)^!p_O[|}!b!t!v#u$Y$p%t&Q&W&e&k&v&{'R'T'Z(|)z*h*i+]+g,P,S,Z,l,t,}-P/V/]0w0z1Q1iU$Z_$])gU$]`$V&ZR)g$[U$ga$z9Td)m$g)n0n4W4z6h8]8x8y9gQ)n$mQ0n+yQ4W0pQ4z1kQ6h4|Q8]8cQ8x8^Q8y9hT9g/S8dQ)p$nR.w)pQ)s$nQ.x)pT.{)s.xQ5q3XR6z5qU*|%|/S9TS0O*|8zR8z8lQ+P%}S0Q+P0SR0S+RU*^%S*U8gR/k*^Q/^)|R3e/^Q6j4}R7_6jQ5Q1mQ6^4tU6p5Q6^7YR7Y6dW)R#y*i.X3oR._)RU.Y(})S/rR2s.YQ1R,`R4e1R[*m%b%c)d0a0i0xR/x*mQ|OU%s|%t,ZS%t}*iR,Z&{Q,S&vQ0w,PT0y,S0wQ0t+{R4^0tQ,V&wR0{,VS%gs4bR*q%gdtO|}%t&{'T'Z*i,Z,}R%ltQ/o*cR3n/o#t!PO[_d|}!b!m!t!v!{#O#u#v$Y$p$s%Q%V%t&Q&W&e&k&s&v&{'R'T'Z'r'v(v(|)q)z*h*i+]+c+g,P,S,Z,b,l,t,}-P-^.z.}/V/]/e0w0z1Q1U1i4f5p9n9oR%v!PQ2S-[R5`2SQ/f*XR3j/fS*[%R.ZR/i*[S-}(l(mR2k-}W(O#U'y'z-nR-r(OQ5g2cR6w5gT(n#p*h|SO|}!f%e%t&f&j&v&{+m,P,S,Z0d0w3{4R5{6|7P7cj$`ae%W%X)y+W/Q0W3u4[6U6n7bW$qd%V'r0fY%Ri%Y'x(x*aQ%y!TQ%{!UQ&l!iQ&u!nQ&x!qQ'q!}S'w#P*yQ(W#[Q(X#^Q(a#aQ(b#eQ(c#fQ(d#gQ(e#hQ(f#iQ(g#jQ(h#kQ(i#lQ(j#mQ(k#nS(l#p*hQ(w#wQ(}#yQ)U#{Q)v$oQ*V%QQ*x%rS+T&P/UQ+f&eS+q&k0zQ,]'OQ,o'UQ-f'pS.Z)O/sQ/P)wS/b*T2^Q/h*ZQ/r*iQ/t*jQ/|*wS0T+S+UQ0b+jQ0j+sQ2`-iQ2a-lQ2g-vQ2i-|Q2o.VQ2q.XQ2u.^Q2v.`Q3V.|Q3b/YQ3g/eQ3t0UQ3}0hQ4Q0kQ4X0rQ5V1rQ5W1uQ5^2QQ5s3aQ5u3oQ5w3wQ5z4OQ5}4PQ6T4YS6c4w5RQ6k5OQ6{5yS7X6a6eQ7a6lS7g7W7ZR7n7iR*Y%Qd]O|}%t&{'T'Z*i,Z,}!j^[_`!b!t!v#u$V$Y$[$]$p&Q&W&Z&e&k&v'R(|)g)z*h+]+g,P,S,l,t-P/V/]0w0z1Q1i#p$ead!m$f$g$m$o$s$v$z%Q%V&P&s'r)l)n)w*P*R+S+U+c+y.v.}/U/`/e0U0m0n0p0r1k4V4W4Y4y4z4|5O5p6R6g6h6l7^8[8]8^8c8d8v8w8x8y9Q9R9f9g9hQ%ktW)r$n)p)s.xW*{%|*|8l8zW+O%}+P+R0SQ.z)qS3_/S9TS6i4}6jR9o9n``O|}%t&{'T*i,ZQ$V[Q$[_`$vd%Q%V'r+c.}/e5p!^&Z!b!t!v#u$Y$p&Q&W&e&k&v'R'Z(|)z*h+]+g,P,S,l,t,}-P/V/]0w0z1Q1iQ&t!mS'o!{,bQ'u#OS(u#v'vQ*P$sQ+w&sQ.U(vQ.y)qQ3U.zQ4g1UQ6W4fQ9S9nR9W9oQ'[!wQ'a!xQ'g!yS'j!z,cQ,q'YQ-U'`Q-Y'eQ-a'iQ-e'nQ1_,lQ1g,sQ4l1`Q4m1aQ4n1bQ4o1dR6[4pR,r'YT,|'Z,}R$PYe.k)Z.g.h.i.l2y2z2{3O5ldmO|}%t&W&{'T*i,Z,lS$W[+]Q&a!bQ'S!tQ'V!vQ(t#uQ)f$Y^){$p&Q'R*h/V/]1QQ+h&eQ+r&kY,Q&v,P,S0w0zS,v'Z,}Q.W(|Q/R)zQ0`+gS1h,t-PR4x1id]O|}%t&{'T'Z*i,Z,}!j^[_`!b!t!v#u$V$Y$[$]$p&Q&W&Z&e&k&v'R(|)g)z*h+]+g,P,S,l,t-P/V/]0w0z1Q1iR%ktQ1m,vQ4t1hQ4{1kQ5T1nQ5U1oQ6d4xU6f4y4z4|Q6o5PS7]6g6hR7j7^X)}$p&Q*h/VpcOtu|}%m%t&{'T'Z*i,Z,g,w,}-P[$ha$z/S8c8d9TU$td${/V^$xef/W3]8e8f9XQ%bmQ'Q!tQ)d$Wb)k$f$g$m8[8]8^9f9g9hQ,e'SQ,p'VQ.t)f[.u)l)n8v8w8x8yQ/X){Q/v*lQ0a+hQ0i+rS0x,Q0`U3R.v9Q9RR3^/RR3Y.}Q&O!XQ*S$vU*}%|/S9TS+X&Q/VW+Y&R/W3]9XQ+b&aQ.R(tQ/T){S/a*P*RQ0P*|Q2p.WQ3T.yQ3[/RQ3f/`Q5n3UQ8{8lQ8|8mQ9U8zQ9V9SR9Y9WX%Ui$}/U/sT)T#y*iR,a'RQ,`'RR4d1Q^zO|}%t&{*i,ZR,n'TW%dm+h+r,QT)e$W0`_{O|}%t&{*i,Z^zO|}%t&{*i,ZQ&i!fQ*p%eQ+l&fQ+p&jQ0e+mQ3z0dQ5x3{Q6P4RQ7O5{Q7d6|Q7e7PR7l7cvrO|}!f%e%t&f&j&{*i+m,Z0d3{4R5{6|7P7cX,R&v,P,S0wQ,O&tR0l+wS+{&t+wQ0o+yQ0u+|U4U0m0n0pQ4]0sS6Q4V4WR7Q6R^vO|}%t&{*i,ZQ,i'TT,x'Z,}R*d%^^xO|}%t&{*i,ZQ,j'TT,y'Z,}^yO|}%t&{*i,ZT,z'Z,}Q-`'gR2X-aR-]'fR's!}[%[i%Y'x(x)O/sR/l*aQ(R#US-m'y'zR2b-nR-q'{R2d-o",
     nodeNames: "⚠ RawString > MacroName LineComment BlockComment PreprocDirective #include String EscapeSequence SystemLibString Identifier ArgumentList ( ConditionalExpression AssignmentExpression CallExpression PrimitiveType FieldExpression FieldIdentifier DestructorName TemplateMethod ScopedFieldIdentifier NamespaceIdentifier TemplateType TypeIdentifier ScopedTypeIdentifier ScopedNamespaceIdentifier :: NamespaceIdentifier TypeIdentifier TemplateArgumentList < TypeDescriptor const volatile restrict _Atomic mutable constexpr StructSpecifier struct MsDeclspecModifier __declspec ) VirtualSpecifier BaseClassClause Access , FieldDeclarationList { FieldDeclaration Attribute AttributeName Identifier AttributeArgs } [ ] UpdateOp ArithOp ArithOp ArithOp LogicOp BitOp BitOp BitOp CompareOp CompareOp CompareOp > CompareOp BitOp UpdateOp Number CharLiteral AttributeArgs virtual extern static register inline AttributeSpecifier __attribute__ PointerDeclarator MsBasedModifier __based MsPointerModifier FunctionDeclarator ParameterList ParameterDeclaration PointerDeclarator FunctionDeclarator Noexcept noexcept ThrowSpecifier throw TrailingReturnType AbstractPointerDeclarator AbstractFunctionDeclarator AbstractArrayDeclarator AbstractParenthesizedDeclarator AbstractReferenceDeclarator ArrayDeclarator ParenthesizedDeclarator ReferenceDeclarator ScopedIdentifier Identifier OperatorName operator ArithOp BitOp CompareOp LogicOp new delete TemplateFunction OperatorName operator StructuredBindingDeclarator OptionalParameterDeclaration VariadicParameterDeclaration VariadicDeclarator ReferenceDeclarator ArrayDeclarator ParenthesizedDeclarator ReferenceDeclarator BitfieldClause InitializerList InitializerPair SubscriptDesignator FieldDesignator TemplateDeclaration template TemplateParameterList TypeParameterDeclaration typename class OptionalTypeParameterDeclaration VariadicTypeParameterDeclaration TemplateTemplateParameterDeclaration AliasDeclaration using Declaration InitDeclarator FriendDeclaration friend FunctionDefinition MsCallModifier CompoundStatement LinkageSpecification DeclarationList CaseStatement case default LabeledStatement StatementIdentifier ExpressionStatement CommaExpression IfStatement if ConditionClause Declaration else SwitchStatement switch DoStatement do while ParenthesizedExpression WhileStatement ForStatement for ReturnStatement return BreakStatement break ContinueStatement continue GotoStatement goto TypeDefinition typedef PointerDeclarator FunctionDeclarator ArrayDeclarator ParenthesizedDeclarator ForRangeLoop TryStatement try CatchClause catch ThrowStatement NamespaceDefinition namespace UsingDeclaration StaticAssertDeclaration static_assert ConcatenatedString TemplateInstantiation FunctionDefinition ExplicitFunctionSpecifier explicit FieldInitializerList FieldInitializer DefaultMethodClause DeleteMethodClause FunctionDefinition OperatorCast Declaration union FunctionDefinition FunctionDefinition FunctionDefinition FunctionDefinition Declaration FunctionDefinition Declaration AccessSpecifier UnionSpecifier EnumSpecifier enum SizedTypeSpecifier TypeSize EnumeratorList Enumerator ClassSpecifier DependentType Decltype decltype auto ParameterPackExpansion ParameterPackExpansion FieldIdentifier PointerExpression SubscriptExpression BinaryExpression ArithOp LogicOp BitOp UnaryExpression LogicOp BitOp UpdateExpression CastExpression SizeofExpression sizeof CompoundLiteralExpression True False NULL NewExpression new NewDeclarator DeleteExpression delete LambdaExpression LambdaCaptureSpecifier ParameterPackExpansion nullptr this #define PreprocArg #if #ifdef #ifndef #else #endif #elif PreprocDirectiveName Macro Program",
     maxTerm: 380,
     nodeProps: [
       ["group", -31,1,8,11,14,15,16,18,74,75,106,116,117,169,198,234,235,236,240,243,244,245,247,248,249,250,251,254,256,258,259,260,"Expression",-12,17,24,25,26,40,219,220,222,226,227,228,230,"Type",-16,149,152,155,157,159,164,166,170,171,173,175,177,179,187,188,192,"Statement"]
     ],
     propSources: [cppHighlighting],
     skippedNodes: [0,3,4,5,6,7,10,261,262,263,264,265,266,267,268,269,270,307,308],
     repeatNodeCount: 37,
     tokenData: "%0W,TR!SOX$_XY'gYZ,cZ]$_]^)e^p$_pq'gqr,yrs.mst/[tu$_uv!/uvw!1gwx!3^xy!3{yz!4pz{!5e{|!6b|}!8Y}!O!8}!O!P!:x!P!Q!Nr!Q!R#2X!R![#Ew![!]$.t!]!^$0d!^!_$1X!_!`$;|!`!a$<w!a!b$?k!b!c$_!c!n$@`!n!o$Aq!o!w$@`!w!x$Aq!x!}$@`!}#O$D[#O#P$Ew#P#Q%$t#Q#R%&a#R#S$@`#S#T$_#T#i$@`#i#j%'^#j#o$@`#o#p%(u#p#q%)j#q#r%+d#r#s%,X#s~$_$O$hW(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_#|%XU(cW&p#tOY%QZw%Qwx%kx#O%Q#O#P%|#P~%Q#t%pS&p#tOY%kZ#O%k#O#P%|#P~%k#t&RV&p#tOY%kYZ%kZ]%k]^&h^#O%k#O#P%|#P~%k#t&mT&p#tOY%kYZ%kZ#O%k#O#P%|#P~%k#v'TU'vQ&p#tOY&|Zr&|rs%ks#O&|#O#P%|#P~&|,T't^(cW'vQ't'q&p#t)[%WOX$_XY'gYZ(pZ]$_]^)e^p$_pq'gqr$_rs%Qsw$_wx&|x#O$_#O#P*l#P~$_'q(uT't'qXY(pYZ(p]^(ppq(p#O#P)U'q)XQYZ(p]^)_'q)bPYZ(p*q)p^(cW'vQ't'q&p#tOX$_XY)eYZ(pZ]$_]^)e^p$_pq)eqr$_rs%Qsw$_wx&|x#O$_#O#P*l#P~$_*g*qV&p#tOY%kYZ+WZ]%k]^+}^#O%k#O#P%|#P~%k*g+_Y't'q&p#tOX%kXY+WYZ(pZ]%k]^+W^p%kpq+Wq#O%k#O#P*l#P~%k*g,ST&p#tOY%kYZ+WZ#O%k#O#P%|#P~%k+y,jT)X%W't'qXY(pYZ(p]^(ppq(p#O#P)U*m-WY#ep&ZP(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x!_$_!_!`-v!`#O$_#O#P%|#P~$_*m.TW#dp!e&{(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_+_.xU(cW'wQ'u(_&p#tOY%QZw%Qwx%kx#O%Q#O#P%|#P~%Q*q/ed(cW'vQ&p#tOX$_XY/[Zp$_pq/[qr$_rs%Qsw$_wx&|x!c$_!c!}0s!}#O$_#O#P%|#P#T$_#T#W0s#W#X2Q#X#Y:d#Y#]0s#]#^Hj#^#o0s#o~$_*q1O`(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#o0s#o~$_*q2]b(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#X0s#X#Y3e#Y#o0s#o~$_*q3pb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#Y0s#Y#Z4x#Z#o0s#o~$_*q5Tb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#]0s#]#^6]#^#o0s#o~$_*q6hb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#b0s#b#c7p#c#o0s#o~$_*q7{b(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#X0s#X#Y9T#Y#o0s#o~$_*q9b`(cW'vQ&o'q&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#o0s#o~$_*q:od(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#`0s#`#a;}#a#b0s#b#cCO#c#o0s#o~$_*q<Yd(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#]0s#]#^=h#^#g0s#g#h@[#h#o0s#o~$_*q=sb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#Y0s#Y#Z>{#Z#o0s#o~$_*q?Y`(cW'vQ&p#t&v'q&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#o0s#o~$_*q@gb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#X0s#X#YAo#Y#o0s#o~$_*qA|`(cW'vQ&t'q&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#o0s#o~$_*qCZb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#W0s#W#XDc#X#o0s#o~$_*qDnb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#]0s#]#^Ev#^#o0s#o~$_*qFRb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#Y0s#Y#ZGZ#Z#o0s#o~$_*qGh`(cW'vQ&p#t&u'q&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#o0s#o~$_*qHud(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#Y0s#Y#ZJT#Z#b0s#b#c!'c#c#o0s#o~$_*qJbd(cW'vQ&q'q&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#W0s#W#XKp#X#b0s#b#c! w#c#o0s#o~$_*qK{b(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#X0s#X#YMT#Y#o0s#o~$_*qM`b(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#Y0s#Y#ZNh#Z#o0s#o~$_*qNu`(cW'vQ&r'q&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#o0s#o~$_*q!!Sb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#W0s#W#X!#[#X#o0s#o~$_*q!#gb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#X0s#X#Y!$o#Y#o0s#o~$_*q!$zb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#Y0s#Y#Z!&S#Z#o0s#o~$_*q!&a`(cW'vQ&s'q&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#o0s#o~$_*q!'nb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#V0s#V#W!(v#W#o0s#o~$_*q!)Rb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#`0s#`#a!*Z#a#o0s#o~$_*q!*fb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#i0s#i#j!+n#j#o0s#o~$_*q!+yb(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#W0s#W#X!-R#X#o0s#o~$_*q!-^b(cW'vQ&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#X0s#X#Y!.f#Y#o0s#o~$_*q!.s`(cW'vQV'q&p#t&w'qOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![0s![!c$_!c!}0s!}#O$_#O#P%|#P#R$_#R#S0s#S#T$_#T#o0s#o~$_*m!0SY(cW'vQ#bp!`&{&p#tOY$_Zr$_rs%Qsw$_wx&|x!_$_!_!`!0r!`#O$_#O#P%|#P~$_*m!0}W!k'm(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_*m!1tZ(`&{(cW'vQ#cp&p#tOY$_Zr$_rs%Qsv$_vw!2gwx&|x!_$_!_!`!0r!`#O$_#O#P%|#P~$_*m!2tW(_&{#ep(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_)w!3iU(dS'vQ(b&{&p#tOY&|Zr&|rs%ks#O&|#O#P%|#P~&|,T!4WW(cW'vQ]+y&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_$a!4{W|a(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_*m!5rY(]&{(cW'vQ#bp&p#tOY$_Zr$_rs%Qsw$_wx&|x!_$_!_!`!0r!`#O$_#O#P%|#P~$_*m!6o[(cW'vQ#bp!^&{&p#tOY$_Zr$_rs%Qsw$_wx&|x{$_{|!7e|!_$_!_!`!0r!`#O$_#O#P%|#P~$_*m!7pW(cW!]'m'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_*P!8eW!Q'P(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_*m!9[](cW'vQ#bp!^&{&p#tOY$_Zr$_rs%Qsw$_wx&|x}$_}!O!7e!O!_$_!_!`!0r!`!a!:T!a#O$_#O#P%|#P~$_*m!:`W(O'm(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_*P!;T[(cW'vQ&p#t'}&{OY$_Zr$_rs%Qsw$_wx&|x!O$_!O!P!;y!P!Q$_!Q![!=g![#O$_#O#P%|#P~$_*P!<SY(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x!O$_!O!P!<r!P#O$_#O#P%|#P~$_*P!<}W(W'P(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_){!=rl(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx!?jx!Q$_!Q![!=g![!g$_!g!h!HY!h!i!MX!i!n$_!n!o!MX!o!r$_!r!s!HY!s!w$_!w!x!MX!x#O$_#O#P%|#P#X$_#X#Y!HY#Y#Z!MX#Z#`$_#`#a!MX#a#d$_#d#e!HY#e#i$_#i#j!MX#j~$_)s!?qW'vQ&p#tOY&|Zr&|rs%ks!Q&|!Q![!@Z![#O&|#O#P%|#P~&|)s!@dl'vQ!l&{&p#tOY&|Zr&|rs%ksw&|wx!?jx!Q&|!Q![!@Z![!g&|!g!h!B[!h!i!Fw!i!n&|!n!o!Fw!o!r&|!r!s!B[!s!w&|!w!x!Fw!x#O&|#O#P%|#P#X&|#X#Y!B[#Y#Z!Fw#Z#`&|#`#a!Fw#a#d&|#d#e!B[#e#i&|#i#j!Fw#j~&|)s!Bej'vQ!l&{&p#tOY&|Zr&|rs%ks{&|{|!DV|}&|}!O!DV!O!Q&|!Q![!ES![!c&|!c!h!ES!h!i!ES!i!n&|!n!o!Fw!o!w&|!w!x!Fw!x#O&|#O#P%|#P#T&|#T#Y!ES#Y#Z!ES#Z#`&|#`#a!Fw#a#i&|#i#j!Fw#j~&|)s!D^['vQ&p#tOY&|Zr&|rs%ks!Q&|!Q![!ES![!c&|!c!i!ES!i#O&|#O#P%|#P#T&|#T#Z!ES#Z~&|)s!E]h'vQ!l&{&p#tOY&|Zr&|rs%ksw&|wx!DVx!Q&|!Q![!ES![!c&|!c!h!ES!h!i!ES!i!n&|!n!o!Fw!o!w&|!w!x!Fw!x#O&|#O#P%|#P#T&|#T#Y!ES#Y#Z!ES#Z#`&|#`#a!Fw#a#i&|#i#j!Fw#j~&|)s!GQb'vQ!l&{&p#tOY&|Zr&|rs%ks!h&|!h!i!Fw!i!n&|!n!o!Fw!o!w&|!w!x!Fw!x#O&|#O#P%|#P#Y&|#Y#Z!Fw#Z#`&|#`#a!Fw#a#i&|#i#j!Fw#j~&|){!Hel(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx&|x{$_{|!J]|}$_}!O!J]!O!Q$_!Q![!Kb![!c$_!c!h!Kb!h!i!Kb!i!n$_!n!o!MX!o!w$_!w!x!MX!x#O$_#O#P%|#P#T$_#T#Y!Kb#Y#Z!Kb#Z#`$_#`#a!MX#a#i$_#i#j!MX#j~$_){!Jf^(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![!Kb![!c$_!c!i!Kb!i#O$_#O#P%|#P#T$_#T#Z!Kb#Z~$_){!Kmh(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx!DVx!Q$_!Q![!Kb![!c$_!c!h!Kb!h!i!Kb!i!n$_!n!o!MX!o!w$_!w!x!MX!x#O$_#O#P%|#P#T$_#T#Y!Kb#Y#Z!Kb#Z#`$_#`#a!MX#a#i$_#i#j!MX#j~$_){!Mdd(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx&|x!h$_!h!i!MX!i!n$_!n!o!MX!o!w$_!w!x!MX!x#O$_#O#P%|#P#Y$_#Y#Z!MX#Z#`$_#`#a!MX#a#i$_#i#j!MX#j~$_,T# P^(cW'vQ#bp!`&{&p#tOY$_Zr$_rs%Qsw$_wx&|xz$_z{# {{!P$_!P!Q#,r!Q!_$_!_!`!0r!`#O$_#O#P%|#P~$_,T#!UZ(cW'vQ&p#tOY# {YZ#!wZr# {rs##lsw# {wx#(rxz# {z{#*{{#O# {#O#P#&P#P~# {)T#!zROz#!wz{##T{~#!w)T##WTOz#!wz{##T{!P#!w!P!Q##g!Q~#!w)T##lOT)T,R##sX(cW&p#tOY##lYZ#!wZw##lwx#$`xz##lz{#']{#O##l#O#P#&P#P~##l+y#$eV&p#tOY#$`YZ#!wZz#$`z{#$z{#O#$`#O#P#&P#P~#$`+y#%PX&p#tOY#$`YZ#!wZz#$`z{#$z{!P#$`!P!Q#%l!Q#O#$`#O#P#&P#P~#$`+y#%sST)T&p#tOY%kZ#O%k#O#P%|#P~%k+y#&UX&p#tOY#$`YZ#$`Z]#$`]^#&q^z#$`z{#$z{#O#$`#O#P#&P#P~#$`+y#&vV&p#tOY#$`YZ#$`Zz#$`z{#$z{#O#$`#O#P#&P#P~#$`,R#'dZ(cW&p#tOY##lYZ#!wZw##lwx#$`xz##lz{#']{!P##l!P!Q#(V!Q#O##l#O#P#&P#P~##l,R#(`U(cWT)T&p#tOY%QZw%Qwx%kx#O%Q#O#P%|#P~%Q+{#(yX'vQ&p#tOY#(rYZ#!wZr#(rrs#$`sz#(rz{#)f{#O#(r#O#P#&P#P~#(r+{#)mZ'vQ&p#tOY#(rYZ#!wZr#(rrs#$`sz#(rz{#)f{!P#(r!P!Q#*`!Q#O#(r#O#P#&P#P~#(r+{#*iU'vQT)T&p#tOY&|Zr&|rs%ks#O&|#O#P%|#P~&|,T#+U](cW'vQ&p#tOY# {YZ#!wZr# {rs##lsw# {wx#(rxz# {z{#*{{!P# {!P!Q#+}!Q#O# {#O#P#&P#P~# {,T#,YW(cW'vQT)T&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_,T#,}W(cW'vQS)T&p#tOY#,rZr#,rrs#-gsw#,rwx#1lx#O#,r#O#P#.g#P~#,r,R#-pU(cWS)T&p#tOY#-gZw#-gwx#.Sx#O#-g#O#P#.g#P~#-g+y#.ZSS)T&p#tOY#.SZ#O#.S#O#P#.g#P~#.S+y#.nVS)T&p#tOY#.SYZ%kZ]#.S]^#/T^#O#.S#O#P#/k#P~#.S+y#/[TS)T&p#tOY#.SYZ%kZ#O#.S#O#P#.g#P~#.S+y#/rZS)T&p#tOY#.SYZ%kZ]#.S]^#/T^#O#.S#O#P#/k#P#b#.S#b#c#.S#c#f#.S#f#g#0e#g~#.S+y#0lSS)T&p#tOY#.SZ#O#.S#O#P#0x#P~#.S+y#1PXS)T&p#tOY#.SYZ%kZ]#.S]^#/T^#O#.S#O#P#/k#P#b#.S#b#c#.S#c~#.S+{#1uU'vQS)T&p#tOY#1lZr#1lrs#.Ss#O#1l#O#P#.g#P~#1l){#2dr(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx#4nx!O$_!O!P#?O!P!Q$_!Q![#Ew![!g$_!g!h!HY!h!i!MX!i!n$_!n!o!MX!o!r$_!r!s!HY!s!w$_!w!x!MX!x#O$_#O#P%|#P#U$_#U#V#HQ#V#X$_#X#Y!HY#Y#Z!MX#Z#`$_#`#a!MX#a#d$_#d#e!HY#e#i$_#i#j!MX#j#l$_#l#m$*y#m~$_)s#4uW'vQ&p#tOY&|Zr&|rs%ks!Q&|!Q![#5_![#O&|#O#P%|#P~&|)s#5hn'vQ!l&{&p#tOY&|Zr&|rs%ksw&|wx#4nx!O&|!O!P#7f!P!Q&|!Q![#5_![!g&|!g!h!B[!h!i!Fw!i!n&|!n!o!Fw!o!r&|!r!s!B[!s!w&|!w!x!Fw!x#O&|#O#P%|#P#X&|#X#Y!B[#Y#Z!Fw#Z#`&|#`#a!Fw#a#d&|#d#e!B[#e#i&|#i#j!Fw#j~&|)s#7ol'vQ!l&{&p#tOY&|Zr&|rs%ks!Q&|!Q![#9g![!c&|!c!g#9g!g!h#<k!h!i#9g!i!n&|!n!o!Fw!o!r&|!r!s!B[!s!w&|!w!x!Fw!x#O&|#O#P%|#P#T&|#T#X#9g#X#Y#<k#Y#Z#9g#Z#`&|#`#a!Fw#a#d&|#d#e!B[#e#i&|#i#j!Fw#j~&|)s#9pn'vQ!l&{&p#tOY&|Zr&|rs%ksw&|wx#;nx!Q&|!Q![#9g![!c&|!c!g#9g!g!h#<k!h!i#9g!i!n&|!n!o!Fw!o!r&|!r!s!B[!s!w&|!w!x!Fw!x#O&|#O#P%|#P#T&|#T#X#9g#X#Y#<k#Y#Z#9g#Z#`&|#`#a!Fw#a#d&|#d#e!B[#e#i&|#i#j!Fw#j~&|)s#;u['vQ&p#tOY&|Zr&|rs%ks!Q&|!Q![#9g![!c&|!c!i#9g!i#O&|#O#P%|#P#T&|#T#Z#9g#Z~&|)s#<tr'vQ!l&{&p#tOY&|Zr&|rs%ksw&|wx#;nx{&|{|!DV|}&|}!O!DV!O!Q&|!Q![#9g![!c&|!c!g#9g!g!h#<k!h!i#9g!i!n&|!n!o!Fw!o!r&|!r!s!B[!s!w&|!w!x!Fw!x#O&|#O#P%|#P#T&|#T#X#9g#X#Y#<k#Y#Z#9g#Z#`&|#`#a!Fw#a#d&|#d#e!B[#e#i&|#i#j!Fw#j~&|){#?Zn(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![#AX![!c$_!c!g#AX!g!h#Cb!h!i#AX!i!n$_!n!o!MX!o!r$_!r!s!HY!s!w$_!w!x!MX!x#O$_#O#P%|#P#T$_#T#X#AX#X#Y#Cb#Y#Z#AX#Z#`$_#`#a!MX#a#d$_#d#e!HY#e#i$_#i#j!MX#j~$_){#Adn(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx#;nx!Q$_!Q![#AX![!c$_!c!g#AX!g!h#Cb!h!i#AX!i!n$_!n!o!MX!o!r$_!r!s!HY!s!w$_!w!x!MX!x#O$_#O#P%|#P#T$_#T#X#AX#X#Y#Cb#Y#Z#AX#Z#`$_#`#a!MX#a#d$_#d#e!HY#e#i$_#i#j!MX#j~$_){#Cmr(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx#;nx{$_{|!J]|}$_}!O!J]!O!Q$_!Q![#AX![!c$_!c!g#AX!g!h#Cb!h!i#AX!i!n$_!n!o!MX!o!r$_!r!s!HY!s!w$_!w!x!MX!x#O$_#O#P%|#P#T$_#T#X#AX#X#Y#Cb#Y#Z#AX#Z#`$_#`#a!MX#a#d$_#d#e!HY#e#i$_#i#j!MX#j~$_){#FSn(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx#4nx!O$_!O!P#?O!P!Q$_!Q![#Ew![!g$_!g!h!HY!h!i!MX!i!n$_!n!o!MX!o!r$_!r!s!HY!s!w$_!w!x!MX!x#O$_#O#P%|#P#X$_#X#Y!HY#Y#Z!MX#Z#`$_#`#a!MX#a#d$_#d#e!HY#e#i$_#i#j!MX#j~$_){#HZ](cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x!O$_!O!P#IS!P!Q$_!Q!R#I{!R![#Ew![#O$_#O#P%|#P~$_){#I]Y(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![!=g![#O$_#O#P%|#P~$_){#JWr(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx#4nx!O$_!O!P#?O!P!Q$_!Q![#Ew![!g$_!g!h!HY!h!i!MX!i!n$_!n!o!MX!o!r$_!r!s!HY!s!w$_!w!x!MX!x#O$_#O#P%|#P#U$_#U#V#Lb#V#X$_#X#Y!HY#Y#Z!MX#Z#`$_#`#a!MX#a#d$_#d#e!HY#e#i$_#i#j!MX#j#l$_#l#m#MZ#m~$_){#LkY(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![#Ew![#O$_#O#P%|#P~$_){#Md^(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![#N`![!c$_!c!i#N`!i#O$_#O#P%|#P#T$_#T#Z#N`#Z~$_){#Nkp(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx$!ox!O$_!O!P#?O!P!Q$_!Q![#N`![!c$_!c!g#N`!g!h$(a!h!i#N`!i!n$_!n!o!MX!o!r$_!r!s!HY!s!w$_!w!x!MX!x#O$_#O#P%|#P#T$_#T#X#N`#X#Y$(a#Y#Z#N`#Z#`$_#`#a!MX#a#d$_#d#e!HY#e#i$_#i#j!MX#j~$_)s$!v['vQ&p#tOY&|Zr&|rs%ks!Q&|!Q![$#l![!c&|!c!i$#l!i#O&|#O#P%|#P#T&|#T#Z$#l#Z~&|)s$#up'vQ!l&{&p#tOY&|Zr&|rs%ksw&|wx$!ox!O&|!O!P#7f!P!Q&|!Q![$#l![!c&|!c!g$#l!g!h$%y!h!i$#l!i!n&|!n!o!Fw!o!r&|!r!s!B[!s!w&|!w!x!Fw!x#O&|#O#P%|#P#T&|#T#X$#l#X#Y$%y#Y#Z$#l#Z#`&|#`#a!Fw#a#d&|#d#e!B[#e#i&|#i#j!Fw#j~&|)s$&Ss'vQ!l&{&p#tOY&|Zr&|rs%ksw&|wx$!ox{&|{|!DV|}&|}!O!DV!O!P#7f!P!Q&|!Q![$#l![!c&|!c!g$#l!g!h$%y!h!i$#l!i!n&|!n!o!Fw!o!r&|!r!s!B[!s!w&|!w!x!Fw!x#O&|#O#P%|#P#T&|#T#X$#l#X#Y$%y#Y#Z$#l#Z#`&|#`#a!Fw#a#d&|#d#e!B[#e#i&|#i#j!Fw#j~&|){$(ls(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx$!ox{$_{|!J]|}$_}!O!J]!O!P#?O!P!Q$_!Q![#N`![!c$_!c!g#N`!g!h$(a!h!i#N`!i!n$_!n!o!MX!o!r$_!r!s!HY!s!w$_!w!x!MX!x#O$_#O#P%|#P#T$_#T#X#N`#X#Y$(a#Y#Z#N`#Z#`$_#`#a!MX#a#d$_#d#e!HY#e#i$_#i#j!MX#j~$_){$+Sa(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x!O$_!O!P#IS!P!Q$_!Q!R$,X!R![#N`![!c$_!c!i#N`!i#O$_#O#P%|#P#T$_#T#Z#N`#Z~$_){$,dt(cW'vQ!l&{&p#tOY$_Zr$_rs%Qsw$_wx$!ox!O$_!O!P#?O!P!Q$_!Q![#N`![!c$_!c!g#N`!g!h$(a!h!i#N`!i!n$_!n!o!MX!o!r$_!r!s!HY!s!w$_!w!x!MX!x#O$_#O#P%|#P#T$_#T#U#N`#U#V#N`#V#X#N`#X#Y$(a#Y#Z#N`#Z#`$_#`#a!MX#a#d$_#d#e!HY#e#i$_#i#j!MX#j#l$_#l#m#MZ#m~$_*q$/PY'|&{(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x![$_![!]$/o!]#O$_#O#P%|#P~$_*q$/zWl'q(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_){$0oW(a&{(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_,P$1f[p&{#dp(cW'vQ&p#tOY$2[Zr$2[rs$3Tsw$2[wx$7Tx!^$2[!^!_$9U!_!`$;P!`!a$8a!a#O$2[#O#P$4p#P~$2[%b$2eY(cW'vQ&p#tOY$2[Zr$2[rs$3Tsw$2[wx$7Tx!`$2[!`!a$8a!a#O$2[#O#P$4p#P~$2[%`$3[W(cW&p#tOY$3TZw$3Twx$3tx!`$3T!`!a$6h!a#O$3T#O#P$4p#P~$3T%W$3yU&p#tOY$3tZ!`$3t!`!a$4]!a#O$3t#O#P$4p#P~$3t%W$4dSY!b&p#tOY%kZ#O%k#O#P%|#P~%k%W$4uV&p#tOY$3tYZ$3tZ]$3t]^$5[^#O$3t#O#P$5v#P~$3t%W$5aV&p#tOY$3tYZ%kZ!`$3t!`!a$4]!a#O$3t#O#P$4p#P~$3t%W$5{X&p#tOY$3tYZ%kZ]$3t]^$5[^!`$3t!`!a$4]!a#O$3t#O#P$4p#P~$3t%`$6qUY!b(cW&p#tOY%QZw%Qwx%kx#O%Q#O#P%|#P~%Q%Y$7[W'vQ&p#tOY$7TZr$7Trs$3ts!`$7T!`!a$7t!a#O$7T#O#P$4p#P~$7T%Y$7}UY!b'vQ&p#tOY&|Zr&|rs%ks#O&|#O#P%|#P~&|%b$8lWY!b(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_,P$9cZ(cW'vQ#cp!j&{&p#tOY$2[Zr$2[rs$3Tsw$2[wx$7Tx!_$2[!_!`$:U!`!a$8a!a#O$2[#O#P$4p#P~$2[,P$:aY!k'm(cW'vQ&p#tOY$2[Zr$2[rs$3Tsw$2[wx$7Tx!`$2[!`!a$8a!a#O$2[#O#P$4p#P~$2[,P$;^Y#dp!f&{(cW'vQ&p#tOY$2[Zr$2[rs$3Tsw$2[wx$7Tx!`$2[!`!a$8a!a#O$2[#O#P$4p#P~$2[*m$<XY(mq(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x!_$_!_!`-v!`#O$_#O#P%|#P~$_*m$=UZ#dp(cW'vQ!h&{&p#tOY$_Zr$_rs%Qsw$_wx&|x!_$_!_!`$=w!`!a$>n!a#O$_#O#P%|#P~$_*m$>UW#dp!f&{(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_*m$>{Y(cW'vQ#cp!j&{&p#tOY$_Zr$_rs%Qsw$_wx&|x!_$_!_!`!0r!`#O$_#O#P%|#P~$_$P$?vW'{P(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_,T$@o`(cW(PS'vQ!W&z'x#T&p#tOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![$@`![!c$_!c!}$@`!}#O$_#O#P%|#P#R$_#R#S$@`#S#T$_#T#o$@`#o~$_,T$BQ`(cW(PS'vQ!W&z'x#T&p#tOY$_Zr$_rs$CSsw$_wx$Cox!Q$_!Q![$@`![!c$_!c!}$@`!}#O$_#O#P%|#P#R$_#R#S$@`#S#T$_#T#o$@`#o~$_+]$C]U(cW'u(_&p#tOY%QZw%Qwx%kx#O%Q#O#P%|#P~%Q)s$CxU'vQ(b&{&p#tOY&|Zr&|rs%ks#O&|#O#P%|#P~&|*m$DgX!Z'm(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x!}$_!}#O$ES#O#P%|#P~$_$P$E_W(YP(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_*q$E|_&p#tOY$F{YZ$G`Z]$F{]^$HX^!Q$F{!Q![$Ho![!w$F{!w!x$Is!x#O$F{#O#P% w#P#i$F{#i#j$Lu#j#l$F{#l#m%!e#m~$F{$O$GSSXY&p#tOY%kZ#O%k#O#P%|#P~%k*q$GiYXY't'q&p#tOX%kXY+WYZ(pZ]%k]^+W^p%kpq+Wq#O%k#O#P*l#P~%k*q$H`TXY&p#tOY%kYZ+WZ#O%k#O#P%|#P~%k$O$HvUXY&p#tOY%kZ!Q%k!Q![$IY![#O%k#O#P%|#P~%k$O$IaUXY&p#tOY%kZ!Q%k!Q![$F{![#O%k#O#P%|#P~%k$O$IxY&p#tOY%kZ!Q%k!Q![$Jh![!c%k!c!i$Jh!i#O%k#O#P%|#P#T%k#T#Z$Jh#Z~%k$O$JmY&p#tOY%kZ!Q%k!Q![$K]![!c%k!c!i$K]!i#O%k#O#P%|#P#T%k#T#Z$K]#Z~%k$O$KbY&p#tOY%kZ!Q%k!Q![$LQ![!c%k!c!i$LQ!i#O%k#O#P%|#P#T%k#T#Z$LQ#Z~%k$O$LVY&p#tOY%kZ!Q%k!Q![$Lu![!c%k!c!i$Lu!i#O%k#O#P%|#P#T%k#T#Z$Lu#Z~%k$O$LzY&p#tOY%kZ!Q%k!Q![$Mj![!c%k!c!i$Mj!i#O%k#O#P%|#P#T%k#T#Z$Mj#Z~%k$O$MoY&p#tOY%kZ!Q%k!Q![$N_![!c%k!c!i$N_!i#O%k#O#P%|#P#T%k#T#Z$N_#Z~%k$O$NdY&p#tOY%kZ!Q%k!Q![% S![!c%k!c!i% S!i#O%k#O#P%|#P#T%k#T#Z% S#Z~%k$O% XY&p#tOY%kZ!Q%k!Q![$F{![!c%k!c!i$F{!i#O%k#O#P%|#P#T%k#T#Z$F{#Z~%k$O%!OVXY&p#tOY%kYZ%kZ]%k]^&h^#O%k#O#P%|#P~%k$O%!jY&p#tOY%kZ!Q%k!Q![%#Y![!c%k!c!i%#Y!i#O%k#O#P%|#P#T%k#T#Z%#Y#Z~%k$O%#_Y&p#tOY%kZ!Q%k!Q![%#}![!c%k!c!i%#}!i#O%k#O#P%|#P#T%k#T#Z%#}#Z~%k$O%$UYXY&p#tOY%kZ!Q%k!Q![%#}![!c%k!c!i%#}!i#O%k#O#P%|#P#T%k#T#Z%#}#Z~%k*P%%PX![&k(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P#Q%%l#Q~$_$d%%wW(ed(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_*m%&nY(cW'vQ#cp&p#t!c&{OY$_Zr$_rs%Qsw$_wx&|x!_$_!_!`!0r!`#O$_#O#P%|#P~$_,T%'mb(cW(PS'vQ!W&z'x#T&p#tOY$_Zr$_rs$CSsw$_wx$Cox!Q$_!Q!Y$@`!Y!Z$Aq!Z![$@`![!c$_!c!}$@`!}#O$_#O#P%|#P#R$_#R#S$@`#S#T$_#T#o$@`#o~$_){%)QW!S&{(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_*m%)w[(cW'vQ#cp&p#t!b&{OY$_Zr$_rs%Qsw$_wx&|x!_$_!_!`!0r!`#O$_#O#P%|#P#p$_#p#q%*m#q~$_*m%*zW(^&{#ep(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_$a%+oW!Ya(cW'vQ&p#tOY$_Zr$_rs%Qsw$_wx&|x#O$_#O#P%|#P~$_$u%,fa(cW'vQ#cp&[P&p#tOX$_XY%-kZp$_pq%-kqr$_rs%Qsw$_wx&|x!c$_!c!}%.y!}#O$_#O#P%|#P#R$_#R#S%.y#S#T$_#T#o%.y#o~$_$T%-ta(cW'vQ&p#tOX$_XY%-kZp$_pq%-kqr$_rs%Qsw$_wx&|x!c$_!c!}%.y!}#O$_#O#P%|#P#R$_#R#S%.y#S#T$_#T#o%.y#o~$_$T%/U`(cW'vQdT&p#tOY$_Zr$_rs%Qsw$_wx&|x!Q$_!Q![%.y![!c$_!c!}%.y!}#O$_#O#P%|#P#R$_#R#S%.y#S#T$_#T#o%.y#o~$_",
     tokenizers: [rawString, fallback, 0, 1, 2, 3, 4, 5, 6, 7, 8],
     topRules: {"Program":[0,271]},
     dynamicPrecedences: {"84":1,"91":1,"98":1,"104":-10,"105":1,"119":-1,"125":-10,"126":1,"183":1,"186":-10,"227":-1,"231":2,"232":2,"270":-10,"325":3,"369":1,"370":3,"371":1,"372":1},
     specialized: [{term: 316, get: value => spec_identifier[value] || -1},{term: 32, get: value => spec_[value] || -1},{term: 70, get: value => spec_templateArgsEnd[value] || -1},{term: 323, get: value => spec_scopedIdentifier[value] || -1}],
     tokenPrec: 21623
   });

   /**
   A language provider based on the [Lezer C++
   parser](https://github.com/lezer-parser/cpp), extended with
   highlighting and indentation information.
   */
   const cppLanguage = /*@__PURE__*/LRLanguage.define({
       parser: /*@__PURE__*/parser.configure({
           props: [
               /*@__PURE__*/indentNodeProp.add({
                   IfStatement: /*@__PURE__*/continuedIndent({ except: /^\s*({|else\b)/ }),
                   TryStatement: /*@__PURE__*/continuedIndent({ except: /^\s*({|catch)\b/ }),
                   LabeledStatement: flatIndent,
                   CaseStatement: context => context.baseIndent + context.unit,
                   BlockComment: () => -1,
                   CompoundStatement: /*@__PURE__*/delimitedIndent({ closing: "}" }),
                   Statement: /*@__PURE__*/continuedIndent({ except: /^{/ })
               }),
               /*@__PURE__*/foldNodeProp.add({
                   "DeclarationList CompoundStatement EnumeratorList FieldDeclarationList InitializerList": foldInside,
                   BlockComment(tree) { return { from: tree.from + 2, to: tree.to - 2 }; }
               })
           ]
       }),
       languageData: {
           commentTokens: { line: "//", block: { open: "/*", close: "*/" } },
           indentOnInput: /^\s*(?:case |default:|\{|\})$/
       }
   });
   /**
   Language support for C++.
   */
   function cpp() {
       return new LanguageSupport(cppLanguage);
   }

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
     });
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

   };

   window.addEventListener('DOMContentLoaded', () => {
     Module.onRuntimeInitialized = () => {
       EnvInitialized = true;
     };
   });

})();
