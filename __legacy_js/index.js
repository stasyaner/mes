import { parse } from './parser.js';
import { inputStream } from './utils.js';

function main() {
    const input = `
        124;
        "hello";
        <div>asd</div>;
        <img />;
        123;
        1 > 3;
        1 > 3 > 4;
        "ddd";
    `;
    inputStream(input);
    console.log('input', input, '\n');
    const ast = parse();
    console.log('\nResult:');
    console.log(JSON.stringify(ast, null, 2));
}

main();
