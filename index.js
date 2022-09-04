import { parse } from './parser.js';

function main() {
    const input = `
        124;
        "hello";
        <div></div>;
    `;
    console.log('input', input, '\n');
    const ast = parse(input);
    console.log('\nResult:');
    console.log(JSON.stringify(ast, null, 2));
}

main();
