export const EOF = -1;
let cursor = 0;

export function getchar(stream) {
    // console.log('cursor', cursor);
    if (cursor >= stream.length) {
        // return null;
        return EOF;
    }

    const c = stream[cursor];
    cursor++;
    console.log('getchar', c);

    return c;
};

// export function sliceStream(stream) {
//     // cursor = startIndex;
//     return stream.slice(cursor ? cursor - 1 : cursor);
// }

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
