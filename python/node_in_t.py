"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

import cStringIO as StringIO
import struct

class node_in_t(object):
    __slots__ = ["drift_x", "drift_y", "deploy_str", "return_str", "moos_manual_override_str"]

    def __init__(self):
        self.drift_x = 0.0
        self.drift_y = 0.0
        self.deploy_str = ""
        self.return_str = ""
        self.moos_manual_override_str = ""

    def encode(self):
        buf = StringIO.StringIO()
        buf.write(node_in_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">dd", self.drift_x, self.drift_y))
        __deploy_str_encoded = self.deploy_str.encode('utf-8')
        buf.write(struct.pack('>I', len(__deploy_str_encoded)+1))
        buf.write(__deploy_str_encoded)
        buf.write("\0")
        __return_str_encoded = self.return_str.encode('utf-8')
        buf.write(struct.pack('>I', len(__return_str_encoded)+1))
        buf.write(__return_str_encoded)
        buf.write("\0")
        __moos_manual_override_str_encoded = self.moos_manual_override_str.encode('utf-8')
        buf.write(struct.pack('>I', len(__moos_manual_override_str_encoded)+1))
        buf.write(__moos_manual_override_str_encoded)
        buf.write("\0")

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = StringIO.StringIO(data)
        if buf.read(8) != node_in_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return node_in_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = node_in_t()
        self.drift_x, self.drift_y = struct.unpack(">dd", buf.read(16))
        __deploy_str_len = struct.unpack('>I', buf.read(4))[0]
        self.deploy_str = buf.read(__deploy_str_len)[:-1].decode('utf-8', 'replace')
        __return_str_len = struct.unpack('>I', buf.read(4))[0]
        self.return_str = buf.read(__return_str_len)[:-1].decode('utf-8', 'replace')
        __moos_manual_override_str_len = struct.unpack('>I', buf.read(4))[0]
        self.moos_manual_override_str = buf.read(__moos_manual_override_str_len)[:-1].decode('utf-8', 'replace')
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if node_in_t in parents: return 0
        tmphash = (0x7e7b599a77d1c02c) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if node_in_t._packed_fingerprint is None:
            node_in_t._packed_fingerprint = struct.pack(">Q", node_in_t._get_hash_recursive([]))
        return node_in_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

