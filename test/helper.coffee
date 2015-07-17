# setup test framework

chai = require 'chai'
should = chai.should()

chai.config.includeStack = yes

module.exports = {
  chai
  should
}
