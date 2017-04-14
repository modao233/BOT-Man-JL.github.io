# Virtual Network Embedding Algorithm Study

> John Lee, 2017/4/13
>
> A breif introduction to my study on Virtual Network Embedding Algorithm

---

## Intro

- Dr. Zhongbao Zhang
- State Key Laboratory of Networking and Switching Technology
- March, 2016 - Now
- Virtual Network Embedding Algorithm Study
- Publishing a research paper

---

## Problem

- Network Virtualization
- Mapping multiple virtual networks (VNs) to one physical network (PN)
- VN Constraints
  - Topology Constraints
  - Capacity Constraints
  - Location Constraints
- NP Hard (Proved)

>  Yu M, Yi Y, Rexford J et al. Rethinking virtual network embedding: substrate support for path splitting and migration. ACM SIGCOMM Computer Communication Review 2008; 38(2): 17–29.

---

## Real-world Problem

- Infrastructure as a Service (AWS, Ali, Tencent)
- Sharing computing capacity and bandwidth

[float-right]

![VNE-Illustration](Virtual-Network-Embedding-Study/vne-illustration.gif)

> Zhang Z, Su S, Niu X et al. Minimizing electricity cost in geographical virtual network embedding. In IEEE GLOBECOM. pp. 2609–2614.

- PN Properties
  - Capacity Property
  - Domain Property
  - Electricity Price Property
- Minimize cost
- Maximize revenue

---

## Previous Works

- Path splitting and migration
- Topology-Aware Node Ranking
- Energy-Aware Virtual Network Embedding

>  Yu M, Yi Y, Rexford J et al. Rethinking virtual network embedding: substrate support for path splitting and migration. ACM SIGCOMM Computer Communication Review 2008; 38(2): 17–29.
>
> Cheng X, Su S, Zhang Z et al. Virtual Network Embedding Through Topology-Aware Node Ranking. ACM SIGCOMM Computer Communication Review 2011; 41(2): 39–47.
>
>  Su S, Zhang Z, Liu AX et al. Energy-aware virtual network embedding. IEEE Transactions on Networking 2014; 22(5): 1607–1620.

<br>

## Two Drawbacks

- Consider Energy Consumption Only
- Consider Electricity Price Only

---

## Two Factors

- Power Consumption
- Electricity Price

<br>

## Three Keypoints

- Mapping to less active nodes
- Shorten inter-domain links
- Prefer lower electricity price

---

## Mapping Scheme

- **Region Mapping (local clustering)**
- Node Mapping (*node rank* & best fit)
- Intra-domain Link Mapping (active-preferred shortest algorithm)
- Inter-domain Link Mapping (active-preferred shortest algorithm)

---

## Our Works

**Clustering Based** Energy-Aware Virtual Network Embedding, _Xu Liu, Zhongbao Zhang, Junning Li, Sen Su_

[align-center]

[img=max-height:170mm]

![Paper](Virtual-Network-Embedding-Study/paper.png)

---

## Region Mapping (Local Clustering)

### Clustering Coefficient

$$
\overline {Price_{ab}} = \sum_{i \in D_a \cap D_b} (\frac {Price_{max} - Price_{i}}{Price_{max} - Price_{min}})^{1/r}, r \in \mathbb{Z^{+}}
$$

$$
w^{*}_{ab} = \alpha \cdot \frac {Price_{max} - \overline {Price_{ab}}}{Price_{max} - Price_{min}} + (1 - \alpha) \cdot \frac {bw_{ab}}{bw_{max}}, \alpha \in (0, 1)
$$

$$
w_{ab} =
\begin{cases} 
w^{*}_{ab} & D_{a} \bigcap D_{b} \ne \emptyset \\ 
0 & D_{a} \bigcap D_{b} = \emptyset
\end{cases}
$$

---

## Region Mapping (Local Clustering)

### Local Clustering

- init: $cluster = \{ \{ u \} | u \in VitualNodes \}$
- calc $w_{ab}$ matrix
- $if$ $max \{ w_{ab} \} != 0$
  - group $a$ and $b$ into one cluster
  - $goto$ $step 2$
- $else$
  - $break$

---

## Special Thanks

- Ye Peida Academy
- State Key Laboratory of Networking and Switching Technology
- Dr. Zhongbao Zhang

---

<br><br><br><br>

# Thank you for Listening 🙂

---