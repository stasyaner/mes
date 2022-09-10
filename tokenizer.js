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

export function getNextToken() {
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
        c = getchar();
    }
    // console.log('c', c);

    if(isJSXOpened) {
        switch(true) {
            case isOpeningCurlyBrace(c):
            default:
                token.type = 'JSXText';
                token.value += c;
                while(!isOpeningAngleBracket(c = getchar())) {
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
                while(isNumber(c = getchar())) {
                    token.value += c;
                }
                // if (!isSemicolon(c)) {
                    cachedCharacter = c;
                // }
                break;
            case isStringEnclosure(c):
                console.log('isStringEnclosure');
                token.type = 'String';
                // TODO: restrict closing string enclosure to the same type as
                //       the opening one
                while(!isStringEnclosure(c = getchar())) {
                    if (isEOF(c)) {
                        throw new Error(
                            'Unexpected end of input while reading string.'
                        );
                    }
                    token.value += c;
                }
                break;
            case isSpace(c):
                // console.log('isSpace');
                // const slicedInput = sliceStream(input);
                return getNextToken();
            case isSemicolon(c):
                console.log('isSemicolon');
                return { type: ';' };
            case isLinebreak(c):
                // console.log('isLinebreak');
                return getNextToken();
                // for the case of expression statement without semicolon
                // return { type: 'Linebreak' };
            case isEOF(c):
                console.log('isEOF');
                return { type: 'EOF' };
            case isOpeningAngleBracket(c):
                console.log('isOpeningAngleBracket');

                const charAfterOpeningAngleBracket = getchar();
                switch(true) {
                    case isSlash(charAfterOpeningAngleBracket):
                        token.type = 'JSXClosing';
                        break;
                    case isSpace(charAfterOpeningAngleBracket):
                        token.type = 'Relational';
                        token.value = c;
                    default:
                        token.type = 'JSXOpening';
                        token.value += charAfterOpeningAngleBracket;
                        isJSXOpened = true;
                }

                if (token.type !== 'Relational') {
                    while(!isClosingAngleBracket(c = getchar())) {
                        if (isSlash(c)) {
                            if (token.type === 'JSXClosing') {
                                let message = 'Unexpected "/" while reading';
                                message += ' closing JSX token';
                                throw new Error(message);
                            }
                            getchar();
                            token.type = 'JSXSelfClosing';
                            isJSXOpened = false;
                            break;
                        }
                        token.value += c;
                    }
                }
                break;
            case isClosingAngleBracket(c):
                token.type = 'Relational';
                token.value = c;
                break;
            default:
                return getNextToken();
                // throw new Error(`Unexpected token: ${c}`);
                // throw new Error('Unexpected continuation of input.');
                // return null;
        }
    }

    console.log('returnToken', token);

    return token;
}

export function assertTokenAndReadValue(lookahead, ...tokenTypes) {
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
