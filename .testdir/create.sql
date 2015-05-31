CREATE TABLE account (
    id integer NOT NULL,
    name character varying(100)
);

CREATE SEQUENCE account_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.account_id_seq;

ALTER SEQUENCE account_id_seq OWNED BY account.id;

CREATE TABLE company (
    id integer NOT NULL,
    code character varying(10) NOT NULL,
    name character varying(100) NOT NULL,
);


CREATE SEQUENCE company_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER SEQUENCE company_id_seq OWNED BY company.id;
 

INSERT INTO company(code,name) VALUES('XXX','Lambda Inc.');
