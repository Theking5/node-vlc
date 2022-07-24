const test = require("./build/release/addon");

console.log("asdasd")

test.play(result => console.log(result))
console.log("asdasd");

setTimeout(() => {
    console.log("asdasd");
}, 5000);