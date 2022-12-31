--
-- PostgreSQL database dump
--

-- Dumped from database version 14.6 (Homebrew)
-- Dumped by pg_dump version 14.6 (Homebrew)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: announcements; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.announcements (
    id integer NOT NULL,
    title character varying(255),
    content character varying(255)
);


ALTER TABLE public.announcements OWNER TO postgres;

--
-- Name: announcements_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.announcements_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.announcements_id_seq OWNER TO postgres;

--
-- Name: announcements_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.announcements_id_seq OWNED BY public.announcements.id;


--
-- Name: auth_user; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.auth_user (
    id integer NOT NULL,
    username character varying(255) NOT NULL,
    password character varying(255) NOT NULL,
    is_superuser boolean NOT NULL,
    first_name character varying(255) NOT NULL,
    last_name character varying(255) NOT NULL,
    email character varying(255) NOT NULL,
    is_active boolean NOT NULL,
    roal character varying(255) NOT NULL
);


ALTER TABLE public.auth_user OWNER TO postgres;

--
-- Name: auth_user_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.auth_user_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.auth_user_id_seq OWNER TO postgres;

--
-- Name: auth_user_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.auth_user_id_seq OWNED BY public.auth_user.id;


--
-- Name: comments; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.comments (
    comment_id integer NOT NULL,
    id integer NOT NULL,
    movie_id integer NOT NULL,
    rating integer,
    content character varying(255)
);


ALTER TABLE public.comments OWNER TO postgres;

--
-- Name: comments_comment_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.comments_comment_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.comments_comment_id_seq OWNER TO postgres;

--
-- Name: comments_comment_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.comments_comment_id_seq OWNED BY public.comments.comment_id;


--
-- Name: movies; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.movies (
    movie_id integer NOT NULL,
    moviename character varying(255),
    starname character varying(255),
    detail character varying(255),
    running_time integer,
    type character varying(255),
    avg_rating numeric,
    poster character varying(255),
    box_office integer,
    num_participants integer,
    release_date integer,
    box_office_unit character varying(255),
    foreign_name character varying(255),
    location character varying(255)
);


ALTER TABLE public.movies OWNER TO postgres;

--
-- Name: movies_movie_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.movies_movie_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.movies_movie_id_seq OWNER TO postgres;

--
-- Name: movies_movie_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.movies_movie_id_seq OWNED BY public.movies.movie_id;


--
-- Name: screening_rooms; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.screening_rooms (
    room_id integer NOT NULL,
    room_name character varying(255),
    theater_id integer NOT NULL,
    capacity integer
);


ALTER TABLE public.screening_rooms OWNER TO postgres;

--
-- Name: screening_rooms_room_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.screening_rooms_room_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.screening_rooms_room_id_seq OWNER TO postgres;

--
-- Name: screening_rooms_room_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.screening_rooms_room_id_seq OWNED BY public.screening_rooms.room_id;


--
-- Name: screenings; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.screenings (
    screening_id integer NOT NULL,
    room_id integer NOT NULL,
    movie_id integer NOT NULL,
    "time" character varying(255),
    price integer,
    showing_date character varying(255)
);


ALTER TABLE public.screenings OWNER TO postgres;

--
-- Name: screenings_screening_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.screenings_screening_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.screenings_screening_id_seq OWNER TO postgres;

--
-- Name: screenings_screening_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.screenings_screening_id_seq OWNED BY public.screenings.screening_id;


--
-- Name: seats; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.seats (
    seat_id integer NOT NULL,
    screening_id integer,
    seat_name character varying(255),
    user_id integer
);


ALTER TABLE public.seats OWNER TO postgres;

--
-- Name: seats_seat_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.seats_seat_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.seats_seat_id_seq OWNER TO postgres;

--
-- Name: seats_seat_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.seats_seat_id_seq OWNED BY public.seats.seat_id;


--
-- Name: theaters; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.theaters (
    theater_id integer NOT NULL,
    theater_name character varying(255),
    theater_address character varying(255)
);


ALTER TABLE public.theaters OWNER TO postgres;

--
-- Name: theaters_theater_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.theaters_theater_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.theaters_theater_id_seq OWNER TO postgres;

--
-- Name: theaters_theater_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.theaters_theater_id_seq OWNED BY public.theaters.theater_id;


--
-- Name: tickets; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tickets (
    ticket_id integer NOT NULL,
    user_id integer NOT NULL,
    screening_id integer NOT NULL,
    seat_id integer NOT NULL,
    price integer NOT NULL,
    refunded boolean NOT NULL
);


ALTER TABLE public.tickets OWNER TO postgres;

--
-- Name: tickets_ticket_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tickets_ticket_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tickets_ticket_id_seq OWNER TO postgres;

--
-- Name: tickets_ticket_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tickets_ticket_id_seq OWNED BY public.tickets.ticket_id;


--
-- Name: announcements id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.announcements ALTER COLUMN id SET DEFAULT nextval('public.announcements_id_seq'::regclass);


--
-- Name: auth_user id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.auth_user ALTER COLUMN id SET DEFAULT nextval('public.auth_user_id_seq'::regclass);


--
-- Name: comments comment_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.comments ALTER COLUMN comment_id SET DEFAULT nextval('public.comments_comment_id_seq'::regclass);


--
-- Name: movies movie_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.movies ALTER COLUMN movie_id SET DEFAULT nextval('public.movies_movie_id_seq'::regclass);


--
-- Name: screening_rooms room_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.screening_rooms ALTER COLUMN room_id SET DEFAULT nextval('public.screening_rooms_room_id_seq'::regclass);


--
-- Name: screenings screening_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.screenings ALTER COLUMN screening_id SET DEFAULT nextval('public.screenings_screening_id_seq'::regclass);


--
-- Name: seats seat_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.seats ALTER COLUMN seat_id SET DEFAULT nextval('public.seats_seat_id_seq'::regclass);


--
-- Name: theaters theater_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.theaters ALTER COLUMN theater_id SET DEFAULT nextval('public.theaters_theater_id_seq'::regclass);


--
-- Name: tickets ticket_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tickets ALTER COLUMN ticket_id SET DEFAULT nextval('public.tickets_ticket_id_seq'::regclass);


--
-- Data for Name: announcements; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.announcements (id, title, content) FROM stdin;
1	a	b
2	Watch the new hits!	Check out Wandering Earth featuring Wu Jing
\.


--
-- Data for Name: auth_user; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.auth_user (id, username, password, is_superuser, first_name, last_name, email, is_active, roal) FROM stdin;
1	admin	9yod80hbsscwIp7v$ncDRV6rkxLm9CfZE0+8+Pi5ZrsaV42powxsuQJ07qvo=	f			admin@zxj.com	t	admin
2	zxj	r0LX03ExdhaFgy55$NBt4x38HXuDC3KmUiFpx0d1yb+613foVNRutzg/KmZI=	f			zxj@zxj.com	t	user
9	wzy	bDuRW5trSbQoPYnp$vNMMZ2qYJNFG3DI3odfJOMQLkmOmNSSYer7sJNz30k8=	f			wzy@wzy.com	t	user
\.


--
-- Data for Name: comments; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.comments (comment_id, id, movie_id, rating, content) FROM stdin;
3	1	1	1	Not very good
2	2	1	4	Very good
4	1	2	4	Very heartwarming
\.


--
-- Data for Name: movies; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.movies (movie_id, moviename, starname, detail, running_time, type, avg_rating, poster, box_office, num_participants, release_date, box_office_unit, foreign_name, location) FROM stdin;
1	Avatar	James Cameron	The Ways of Water	120	Sci-fi	2.5000000000000000		60	3	2022		A	Shanghai
2	Wandering Earth	Wu Jing	A Chinese Sci-fi Movie	130	Sci-fi	4.0000000000000000		20	1	2021		WE	Shanghai
\.


--
-- Data for Name: screening_rooms; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.screening_rooms (room_id, room_name, theater_id, capacity) FROM stdin;
5	Shanghai 3D	1	20
3	Shanghai IMAX	1	20
\.


--
-- Data for Name: screenings; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.screenings (screening_id, room_id, movie_id, "time", price, showing_date) FROM stdin;
10	5	1	08:00	20	2023-01-01
11	3	2	08:00	20	2023-01-02
12	3	2	12:00	50	2023-01-02
13	3	2	14:00	90	2023-01-03
14	3	2	18:00	60	2023-01-03
15	5	2	12:00	30	2023-01-04
16	5	2	17:00	40	2023-01-04
17	3	1	15:00	50	2023-01-01
18	3	1	19:00	80	2023-01-01
19	3	1	08:00	40	2023-01-02
20	3	1	18:00	80	2022-01-02
\.


--
-- Data for Name: seats; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.seats (seat_id, screening_id, seat_name, user_id) FROM stdin;
1	10	#0	\N
2	10	#1	\N
4	10	#3	\N
7	10	#6	\N
8	10	#7	\N
9	10	#8	\N
10	10	#9	\N
11	10	#10	\N
12	10	#11	\N
13	10	#12	\N
14	10	#13	\N
15	10	#14	\N
16	10	#15	\N
17	10	#16	\N
18	10	#17	\N
19	10	#18	\N
20	10	#19	\N
3	10	#2	1
6	10	#5	2
5	10	#4	2
21	11	#1	\N
22	11	#2	\N
24	11	#4	\N
25	11	#5	\N
26	11	#6	\N
27	11	#7	\N
28	11	#8	\N
29	11	#9	\N
30	11	#10	\N
31	11	#11	\N
32	11	#12	\N
33	11	#13	\N
34	11	#14	\N
35	11	#15	\N
36	11	#16	\N
37	11	#17	\N
38	11	#18	\N
39	11	#19	\N
40	11	#20	\N
41	12	#1	\N
42	12	#2	\N
43	12	#3	\N
44	12	#4	\N
45	12	#5	\N
46	12	#6	\N
47	12	#7	\N
48	12	#8	\N
49	12	#9	\N
50	12	#10	\N
51	12	#11	\N
52	12	#12	\N
53	12	#13	\N
54	12	#14	\N
55	12	#15	\N
56	12	#16	\N
57	12	#17	\N
58	12	#18	\N
59	12	#19	\N
60	12	#20	\N
61	13	#1	\N
62	13	#2	\N
63	13	#3	\N
64	13	#4	\N
65	13	#5	\N
66	13	#6	\N
67	13	#7	\N
68	13	#8	\N
69	13	#9	\N
70	13	#10	\N
71	13	#11	\N
72	13	#12	\N
73	13	#13	\N
74	13	#14	\N
75	13	#15	\N
76	13	#16	\N
77	13	#17	\N
78	13	#18	\N
79	13	#19	\N
80	13	#20	\N
81	14	#1	\N
82	14	#2	\N
83	14	#3	\N
84	14	#4	\N
85	14	#5	\N
86	14	#6	\N
87	14	#7	\N
88	14	#8	\N
89	14	#9	\N
90	14	#10	\N
91	14	#11	\N
92	14	#12	\N
93	14	#13	\N
94	14	#14	\N
95	14	#15	\N
96	14	#16	\N
97	14	#17	\N
98	14	#18	\N
99	14	#19	\N
100	14	#20	\N
23	11	#3	1
101	15	#1	\N
102	15	#2	\N
103	15	#3	\N
104	15	#4	\N
105	15	#5	\N
106	15	#6	\N
107	15	#7	\N
108	15	#8	\N
109	15	#9	\N
110	15	#10	\N
111	15	#11	\N
112	15	#12	\N
113	15	#13	\N
114	15	#14	\N
115	15	#15	\N
116	15	#16	\N
117	15	#17	\N
118	15	#18	\N
119	15	#19	\N
120	15	#20	\N
121	16	#1	\N
122	16	#2	\N
123	16	#3	\N
124	16	#4	\N
125	16	#5	\N
126	16	#6	\N
127	16	#7	\N
128	16	#8	\N
129	16	#9	\N
130	16	#10	\N
131	16	#11	\N
132	16	#12	\N
133	16	#13	\N
134	16	#14	\N
135	16	#15	\N
136	16	#16	\N
137	16	#17	\N
138	16	#18	\N
139	16	#19	\N
140	16	#20	\N
141	17	#1	\N
142	17	#2	\N
143	17	#3	\N
144	17	#4	\N
145	17	#5	\N
146	17	#6	\N
147	17	#7	\N
148	17	#8	\N
149	17	#9	\N
150	17	#10	\N
151	17	#11	\N
152	17	#12	\N
153	17	#13	\N
154	17	#14	\N
155	17	#15	\N
156	17	#16	\N
157	17	#17	\N
158	17	#18	\N
159	17	#19	\N
160	17	#20	\N
161	18	#1	\N
162	18	#2	\N
163	18	#3	\N
164	18	#4	\N
165	18	#5	\N
166	18	#6	\N
167	18	#7	\N
168	18	#8	\N
169	18	#9	\N
170	18	#10	\N
171	18	#11	\N
172	18	#12	\N
173	18	#13	\N
174	18	#14	\N
175	18	#15	\N
176	18	#16	\N
177	18	#17	\N
178	18	#18	\N
179	18	#19	\N
180	18	#20	\N
181	19	#1	\N
182	19	#2	\N
183	19	#3	\N
184	19	#4	\N
185	19	#5	\N
186	19	#6	\N
187	19	#7	\N
188	19	#8	\N
189	19	#9	\N
190	19	#10	\N
191	19	#11	\N
192	19	#12	\N
193	19	#13	\N
194	19	#14	\N
195	19	#15	\N
196	19	#16	\N
197	19	#17	\N
198	19	#18	\N
199	19	#19	\N
200	19	#20	\N
201	20	#1	\N
202	20	#2	\N
203	20	#3	\N
204	20	#4	\N
205	20	#5	\N
206	20	#6	\N
207	20	#7	\N
208	20	#8	\N
209	20	#9	\N
210	20	#10	\N
211	20	#11	\N
212	20	#12	\N
213	20	#13	\N
214	20	#14	\N
215	20	#15	\N
216	20	#16	\N
217	20	#17	\N
218	20	#18	\N
219	20	#19	\N
220	20	#20	\N
\.


--
-- Data for Name: theaters; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.theaters (theater_id, theater_name, theater_address) FROM stdin;
1	Theater 1	China
\.


--
-- Data for Name: tickets; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.tickets (ticket_id, user_id, screening_id, seat_id, price, refunded) FROM stdin;
1	1	10	2	20	t
2	1	10	3	20	f
3	2	10	6	20	f
4	2	10	5	20	f
5	1	11	23	20	f
\.


--
-- Name: announcements_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.announcements_id_seq', 2, true);


--
-- Name: auth_user_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.auth_user_id_seq', 9, true);


--
-- Name: comments_comment_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.comments_comment_id_seq', 4, true);


--
-- Name: movies_movie_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.movies_movie_id_seq', 2, true);


--
-- Name: screening_rooms_room_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.screening_rooms_room_id_seq', 5, true);


--
-- Name: screenings_screening_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.screenings_screening_id_seq', 20, true);


--
-- Name: seats_seat_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.seats_seat_id_seq', 220, true);


--
-- Name: theaters_theater_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.theaters_theater_id_seq', 1, true);


--
-- Name: tickets_ticket_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.tickets_ticket_id_seq', 5, true);


--
-- Name: announcements announcements_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.announcements
    ADD CONSTRAINT announcements_pkey PRIMARY KEY (id);


--
-- Name: auth_user auth_user_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.auth_user
    ADD CONSTRAINT auth_user_pkey PRIMARY KEY (id);


--
-- Name: auth_user auth_user_username_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.auth_user
    ADD CONSTRAINT auth_user_username_key UNIQUE (username);


--
-- Name: comments comments_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.comments
    ADD CONSTRAINT comments_pkey PRIMARY KEY (comment_id);


--
-- Name: movies movies_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.movies
    ADD CONSTRAINT movies_pkey PRIMARY KEY (movie_id);


--
-- Name: screening_rooms screening_rooms_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.screening_rooms
    ADD CONSTRAINT screening_rooms_pkey PRIMARY KEY (room_id);


--
-- Name: screenings screenings_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.screenings
    ADD CONSTRAINT screenings_pkey PRIMARY KEY (screening_id);


--
-- Name: seats seats_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.seats
    ADD CONSTRAINT seats_pkey PRIMARY KEY (seat_id);


--
-- Name: theaters theaters_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.theaters
    ADD CONSTRAINT theaters_pkey PRIMARY KEY (theater_id);


--
-- Name: tickets tickets_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tickets
    ADD CONSTRAINT tickets_pkey PRIMARY KEY (ticket_id);


--
-- Name: comments comments_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.comments
    ADD CONSTRAINT comments_id_fkey FOREIGN KEY (id) REFERENCES public.auth_user(id);


--
-- Name: comments comments_movie_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.comments
    ADD CONSTRAINT comments_movie_id_fkey FOREIGN KEY (movie_id) REFERENCES public.movies(movie_id);


--
-- Name: screening_rooms screening_rooms_theater_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.screening_rooms
    ADD CONSTRAINT screening_rooms_theater_id_fkey FOREIGN KEY (theater_id) REFERENCES public.theaters(theater_id);


--
-- Name: screenings screenings_movie_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.screenings
    ADD CONSTRAINT screenings_movie_id_fkey FOREIGN KEY (movie_id) REFERENCES public.movies(movie_id);


--
-- Name: screenings screenings_room_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.screenings
    ADD CONSTRAINT screenings_room_id_fkey FOREIGN KEY (room_id) REFERENCES public.screening_rooms(room_id);


--
-- Name: seats seats_screening_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.seats
    ADD CONSTRAINT seats_screening_id_fkey FOREIGN KEY (screening_id) REFERENCES public.screenings(screening_id);


--
-- Name: seats seats_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.seats
    ADD CONSTRAINT seats_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.auth_user(id);


--
-- Name: tickets tickets_screening_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tickets
    ADD CONSTRAINT tickets_screening_id_fkey FOREIGN KEY (screening_id) REFERENCES public.screenings(screening_id);


--
-- Name: tickets tickets_seat_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tickets
    ADD CONSTRAINT tickets_seat_id_fkey FOREIGN KEY (seat_id) REFERENCES public.seats(seat_id);


--
-- Name: tickets tickets_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tickets
    ADD CONSTRAINT tickets_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.auth_user(id);


--
-- PostgreSQL database dump complete
--

