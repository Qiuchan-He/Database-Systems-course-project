SELECT 1
FROM T AS T0, T AS T1, T AS T2, T AS T3, T AS T4, T AS T5
WHERE T0.fid_T1 = T1.id
  AND T0.fid_T2 = T2.id
  AND T1.fid_T3 = T3.id
  AND T1.fid_T5 = T5.id
  AND T3.fid_T4 = T4.id
  AND T3.fid_T5 = T5.id
;
