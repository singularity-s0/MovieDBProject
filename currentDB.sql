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
    avg_rating integer,
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
    price character varying(255),
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
    id integer NOT NULL,
    screening_id integer NOT NULL,
    "time" integer,
    price integer,
    seat_num integer
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
-- Name: theaters theater_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.theaters ALTER COLUMN theater_id SET DEFAULT nextval('public.theaters_theater_id_seq'::regclass);


--
-- Name: tickets ticket_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tickets ALTER COLUMN ticket_id SET DEFAULT nextval('public.tickets_ticket_id_seq'::regclass);


--
-- Data for Name: auth_user; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.auth_user (id, username, password, is_superuser, first_name, last_name, email, is_active, roal) FROM stdin;
4	a	L0Od3ZsDr36KOiVN$J8ISpjF7mLmRCq8cXe1PJISCaCEiOM4ERBpVWWCWmFI=	f	b	c	name@qq.com	t	1
1	admin	9yod80hbsscwIp7v$ncDRV6rkxLm9CfZE0+8+Pi5ZrsaV42powxsuQJ07qvo=	f			admin@zxj.com	t	admin
2	zxj	r0LX03ExdhaFgy55$NBt4x38HXuDC3KmUiFpx0d1yb+613foVNRutzg/KmZI=	f			zxj@zxj.com	t	user
\.


--
-- Data for Name: comments; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.comments (comment_id, id, movie_id, rating, content) FROM stdin;
\.


--
-- Data for Name: movies; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.movies (movie_id, moviename, starname, detail, running_time, type, avg_rating, poster, box_office, num_participants, release_date, box_office_unit, foreign_name, location) FROM stdin;
1	Avatar	zxj	a new movie	120	sci-fi	0		0	0	0		avatar	shanghai
\.


--
-- Data for Name: screening_rooms; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.screening_rooms (room_id, room_name, theater_id, capacity) FROM stdin;
4	THX	1	25
1	Room 1	1	30
3	IMAX	1	20
\.


--
-- Data for Name: screenings; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.screenings (screening_id, room_id, movie_id, "time", price, showing_date) FROM stdin;
5	1	1	17:00	40	2022-12-31
4	1	1	12:00	30	2022-12-31
6	1	1	23:00	10	2022-12-32
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

COPY public.tickets (ticket_id, id, screening_id, "time", price, seat_num) FROM stdin;
\.


--
-- Name: auth_user_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.auth_user_id_seq', 2, true);


--
-- Name: comments_comment_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.comments_comment_id_seq', 1, false);


--
-- Name: movies_movie_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.movies_movie_id_seq', 1, true);


--
-- Name: screening_rooms_room_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.screening_rooms_room_id_seq', 4, true);


--
-- Name: screenings_screening_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.screenings_screening_id_seq', 6, true);


--
-- Name: theaters_theater_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.theaters_theater_id_seq', 1, true);


--
-- Name: tickets_ticket_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.tickets_ticket_id_seq', 1, false);


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
-- Name: tickets tickets_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tickets
    ADD CONSTRAINT tickets_id_fkey FOREIGN KEY (id) REFERENCES public.auth_user(id);


--
-- Name: tickets tickets_screening_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tickets
    ADD CONSTRAINT tickets_screening_id_fkey FOREIGN KEY (screening_id) REFERENCES public.screenings(screening_id);


--
-- Name: SCHEMA public; Type: ACL; Schema: -; Owner: postgres
--

GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- PostgreSQL database dump complete
--

