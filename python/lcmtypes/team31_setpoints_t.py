"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class team31_setpoints_t(object):
    __slots__ = ["utime", "left_velocity", "right_velocity"]

    __typenames__ = ["int64_t", "float", "float"]

    __dimensions__ = [None, None, None]

    def __init__(self):
        self.utime = 0
        self.left_velocity = 0.0
        self.right_velocity = 0.0

    def encode(self):
        buf = BytesIO()
        buf.write(team31_setpoints_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">qff", self.utime, self.left_velocity, self.right_velocity))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != team31_setpoints_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return team31_setpoints_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = team31_setpoints_t()
        self.utime, self.left_velocity, self.right_velocity = struct.unpack(">qff", buf.read(16))
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if team31_setpoints_t in parents: return 0
        tmphash = (0xa276c2ad1d2ba9a0) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff) + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if team31_setpoints_t._packed_fingerprint is None:
            team31_setpoints_t._packed_fingerprint = struct.pack(">Q", team31_setpoints_t._get_hash_recursive([]))
        return team31_setpoints_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

