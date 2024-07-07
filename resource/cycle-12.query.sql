SELECT T0.id, T1.id, T2.id, T3.id, T4.id, T5.id, T6.id, T7.id, T8.id, T9.id, T10.id, T11.id
FROM T AS T0, T AS T1, T AS T2, T AS T3, T AS T4, T AS T5, T AS T6, T AS T7, T AS T8, T AS T9, T AS T10, T AS T11
WHERE T0.fid_T1 = T1.id
  AND T0.fid_T11 = T11.id
  AND T1.fid_T2 = T2.id
  AND T2.fid_T3 = T3.id
  AND T3.fid_T4 = T4.id
  AND T4.fid_T5 = T5.id
  AND T5.fid_T6 = T6.id
  AND T6.fid_T7 = T7.id
  AND T7.fid_T8 = T8.id
  AND T8.fid_T9 = T9.id
  AND T9.fid_T10 = T10.id
  AND T10.fid_T11 = T11.id
;
