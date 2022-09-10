export const EOF = -1;
let cursor = 0;
let stream = '';

export function inputStream(input) {
    stream = input;
}

export function getchar() {
    // console.log('cursor', cursor);
    if (cursor >= stream.length) {
        // return null;
        return EOF;
    }

    const c = stream[cursor];
    cursor++;
    if (c !== ' ' && c !== '\n') console.log('getchar', c);

    return c;
};


export function isNumber(c) {
    // const charCode = c.charCodeAt(0);
    // return charCode >= 48 && charCode <= 57;
    return c >= '0' && c <= '9';
}

export function isStringEnclosure(c) {
    return c === '"' || c === '\'';
}

export function isSpace(c) {
    return c === ' ';
}

export function isSemicolon(c) {
    return c === ';';
}

export function isLinebreak(c) {
    return c === '\n';
}

export function isEOF(c) {
    return c === EOF;
}

export function isOpeningAngleBracket(c) {
    return c === '<';
}

export function isClosingAngleBracket(c) {
    return c === '>';
}

export function isSlash(c) {
    return c === '/';
}

export function isOpeningCurlyBrace(c) {
    return c === '{';
}

export function isClosingCurlyBrace(c) {
    return c === '}';
}

export function isNotControlCharacter(c) {
    return c >= '!' && c <= '~';
}

export function isJsxText(c) {
    return isNotControlCharacter(c) && !isOpeningCurlyBrace(c) &&
           !isClosingCurlyBrace(c);
}
