# AHRS Calculation


$$
roll  (X): \phi \\
pitch (Y): \theta \\
yaw   (Z): \psi \\
\\
g = 
\begin{bmatrix}
	0\\ 0 \\ g
\end{bmatrix}
\\
\begin{bmatrix}
	a_x \\ a_y \\ a_z
\end{bmatrix}
=
\begin{bmatrix}
	-g sin(\theta) 			\\
	g cos(\theta) sin(\phi) \\
	g cos \theta cos \phi	
\end{bmatrix}
\\
sin(\theta) = \frac{-a_x}{g} \\
tan(\phi) = \frac{a_y}{a_z}
\\
\psi = atan \frac{cos\theta(b_zsin\phi - b_ycos\phi)}{b_x + BsinIsin\theta}
$$
