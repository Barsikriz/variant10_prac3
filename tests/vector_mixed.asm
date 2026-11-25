# First vector: addresses 100..107.
# Second vector: addresses 200..207.
# Result overwrites the first vector.

li r0, 0
sw r0, 100
li r0, 0
sw r0, 200

li r0, 1
sw r0, 101
li r0, 32
sw r0, 201

li r0, 3
sw r0, 102
li r0, 33
sw r0, 202

li r0, 15
sw r0, 103
li r0, 4
sw r0, 203

li r0, 255
sw r0, 104
li r0, 8
sw r0, 204

li r0, 1024
sw r0, 105
li r0, 10
sw r0, 205

li r0, 65535
sw r0, 106
li r0, 16
sw r0, 206

li r0, 2097151
sw r0, 107
li r0, 21
sw r0, 207

li r1, 200
ror r2, 100, r1
sw r2, 100

li r1, 201
ror r2, 101, r1
sw r2, 101

li r1, 202
ror r2, 102, r1
sw r2, 102

li r1, 203
ror r2, 103, r1
sw r2, 103

li r1, 204
ror r2, 104, r1
sw r2, 104

li r1, 205
ror r2, 105, r1
sw r2, 105

li r1, 206
ror r2, 106, r1
sw r2, 106

li r1, 207
ror r2, 107, r1
sw r2, 107

