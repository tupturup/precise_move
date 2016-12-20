var fs = require('fs');
var sql = require('sql.js');
var path = require('path')
var bfr = fs.readFileSync(path.resolve(__dirname, 'tmp', 'db.sqlite'))

//var bfr = fs.readFileSync('./tmp/db.sqlite');
var db = new sql.Database(bfr);
db.each('SELECT * FROM test', function (row) {
  console.log(row);
});
