# Value to rotate: M[100] = 9.
li r0, 9
sw r0, 100

# Rotation count: M[101] = 1.
li r0, 1
sw r0, 101

# r1 points to the rotation count.
li r1, 101

# Rotate M[100] by M[r1], then save the result.
ror r2, 100, r1
sw r2, 200

# Rotation by 32 preserves the value.
li r0, 32
sw r0, 101
ror r2, 100, r1
sw r2, 201

# Rotation by 33 equals rotation by 1.
li r0, 33
sw r0, 101
ror r2, 100, r1
sw r2, 202
