import {
    getchar,
    isNumber,
    isStringEnclosure,
    isSpace,
    isSemicolon,
    isLinebreak,
    isEOF,
    isOpeningAngleBracket,
    isClosingAngleBracket,
    isSlash,
    isClosingCurlyBrace,
    isOpeningCurlyBrace,
} from './utils.js';

let isJSXOpened = false;
let cachedCharacter = null;

export function getNextToken(input) {
    let token = {
        type: null,
        value: '',
    };
    let c = null;
    if(cachedCharacter) {
        console.log('cachedCharacter', cachedCharacter);
        c = cachedCharacter;
        cachedCharacter = null;
    } else {
        c = getchar(input);
    }
    console.log('c', c);

    if(isJSXOpened) {
        switch(true) {
            case isOpeningCurlyBrace(c):
            default:
                token.type = 'JSXText';
                token.value += c;
                while(!isOpeningAngleBracket(c = getchar(input))) {
                    token.value += c;
                    if (isEOF(c)) {
                        throw new Error(
                            'Unexpected end of input while reading JSX text.'
                        );
                    }
                }
                cachedCharacter = c;
                isJSXOpened = false;
        }
    } else {
        switch(true) {
            // case !!c: return null;
            case isNumber(c):
                console.log('isNumber');
                token.type = 'Number';
                token.value += c;
                // Note: maybe it's not right to getchat while number.
                //       What if char in the sequence?
                while(isNumber(c = getchar(input))) {
                    token.value += c;
                }
                // if (!isSemicolon(c)) {
                    cachedCharacter = c;
                // }
                break;
            case isStringEnclosure(c):
                console.log('isStringEnclosure');
                token.type = 'String';
                while(!isStringEnclosure(c = getchar(input))) {
                    if (isEOF(c)) {
                        throw new Error(
                            'Unexpected end of input while reading string.'
                        );
                    }
                    token.value += c;
                }
                break;
            case isSpace(c):
                console.log('isSpace');
                // const slicedInput = sliceStream(input);
                return getNextToken(input);
            case isSemicolon(c):
                console.log('isSemicolon');
                return { type: ';' };
            case isLinebreak(c):
                console.log('isLinebreak');
                return getNextToken(input);
                // for the case of expression statement without semicolon
                // return { type: 'Linebreak' };
            case isEOF(c):
                console.log('isEOF');
                return { type: 'EOF' };
            case isOpeningAngleBracket(c):
                console.log('isOpeningAngleBracket');

                const charAfterOpeningAngleBracket = getchar(input);
                if (isSlash(charAfterOpeningAngleBracket)) {
                    token.type = 'JSXClosing';
                } else {
                    token.type = 'JSXOpening';
                    token.value += charAfterOpeningAngleBracket;
                    isJSXOpened = true;
                }

                while(!isClosingAngleBracket(c = getchar(input))) {
                    token.value += c;
                }
                break;
            default:
                return getNextToken(input);
                // throw new Error(`Unexpected token: ${c}`);
                // throw new Error('Unexpected continuation of input.');
                // return null;
        }
    }

    console.log('returnToken', token);

    return token;
}

export function readToken(lookahead, ...tokenTypes) {
    if (!lookahead) {
        throw new Error("Unexpected end of input.");
    }

    if (!tokenTypes.some(tokenType => tokenType === lookahead.type)) {
    // if (lookahead.type !== tokenType) {
        let message = 'Unexpected token. ';
        message += `Expected [${tokenTypes}], recieved: ${lookahead.type}`;
        throw new Error(message);
    }

    return lookahead.value;
}
