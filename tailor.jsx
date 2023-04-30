import {
    useEffect,
    useState,
    useCallback,
} from "react";
import CV from "../components/tailor/CV";
import {
    Dashboard,
    DashSelector,
} from "../components/tailor/dashboard";
import {
    Col,
    Row,
    Button,
} from "react-bootstrap";
import {
    CvData,
    CvValidation,
} from "../components/tailor/types";
import NoMasterCV from "../components/tailor/noMasterCV";
import {
    getHtmlCv,
    getMyCV,
    removeCVFields,
    saveMyCV,
    deleteCv,
} from "../service/service";
import {
    formatAsCVData,
    formatAsCVType,
} from "../utils/formatters";
import {
    onCloneCV,
    onCreateCV,
    onFieldChange,
    onItemAction,
    onItemMove,
    onSkillAction,
} from "../components/tailor/utils";
import {
    MutationDeleteCvFieldsArgs,
    MutationUpsertCvArgs,
} from "../model/gqlSchemaGenerated";
import {
    differenceBy,
    cloneDeep,
} from "lodash";
import {
    schema,
    SchemaType,
} from "../components/tailor/validationSchema";
import { ValidationError } from "yup";
import { AuthCheckBanner } from "../components";
import Head from "next/head";

const Tailor = () => {
    const [masterCV, setMasterCV] = useState(null);
    const [masterCVDefault, setMasterCVDefault] = useState(null);
    const [specificCV, setSpecificCV] = useState(null);
    const [currentMode, setCurrentMode] = useState("LOADING");
    const [prevMode, setPrevMode] = useState("LOADING");
    const [showLoading, setShowLoading] = useState(true);
    const [showSaving, setShowSaving] = useState(false);
    const [specificCVs, setSpecificCVs] = useState([]);
    const [indexSCV, setIndexSCV] = useState(-1);
    const [validateOnChange, setValidateOnChange] = useState(false);
    const [cvValidation, setCvValidation] = useState({ errors: { }, formState: { isValid: false } });
    const [htmlCv, setHtmlCv] = useState({ cvId: "", html: "" });

    const setMode = useCallback((mode) => {
        setPrevMode(currentMode);
        setCurrentMode(mode);
    }, [currentMode]);

    const validateCV = (cv) =>
        schema.validate(cv, { abortEarly: false, strict: true, context: { cv } });

    const setFormErrors = (err) => {
        const fieldErrors = { };
        const errors = err.inner;
        errors.forEach((e) => {
            if (!e.path) return;
            const path = e.path.split(".");
            const name = path[1]; // field name
            const section = path[0];  // section index
            let index = -1;
            if (section.lastIndexOf("[") !== -1) {
                index = parseInt(section.slice(section.lastIndexOf("[") + 1, section.lastIndexOf("]")), 10);
            }
            if (index === -1) {
                fieldErrors[name] = { message: e.errors.join(", ") };
            } else {
                fieldErrors[name] = fieldErrors[name] || [];
                fieldErrors[name][index] = { message: e.errors.join(", ") };
            }
        });
        setCvValidation({ errors: fieldErrors, formState: { isValid: false } });
    };

    useEffect(() => {
        const cv = currentMode === "MASTER_CV" ? masterCV : specificCV;
        if (validateOnChange && cv) {
            validateCV(cv)
                .then(() => setCvValidation({ errors: { }, formState: { isValid: true } }))
                .catch((err) => setFormErrors(err));
        }
    }, [validateOnChange, masterCV, currentMode, specificCV]);

    useEffect(() => {
        (async function() {
            const res = await getMyCV();
            let masterCvRes = null;
            if (res?.length > 0) {
                setMode("DASHBOARD");
                masterCvRes = res.find(e => e.isPrime);
                const cv = formatAsCVData(masterCvRes);
                setMasterCV(cv);
                setMasterCVDefault(cloneDeep(cv));
                const formattedSpecificCvs = res.reduce((cvs, resCv) => {
                    const newCvs = [...cvs];
                    if (!resCv.isPrime) {
                        newCvs.push(formatAsCVData(resCv));
                    }

                    return newCvs;
                }, []);
                setSpecificCVs(formattedSpecificCvs);
            } else {
                setMode("NO_CV");
                const cv = formatAsCVData();
                setMasterCV(cv);
                setMasterCVDefault(cloneDeep(cv));
            }
            if (masterCvRes) {
                const resHtmlCv = await getHtmlCv(masterCvRes.id);
                setHtmlCv({ cvId: masterCvRes.id ?? "", html: resHtmlCv });
            }
            setShowLoading(false);
        })();
    // eslint-disable-next-line react-hooks/exhaustive-deps
    }, []);

    const onEditCV = (ind) => {
        if (ind === undefined) {
            setMode("MASTER_CV");
            return;
        }
        setIndexSCV(ind);
        setSpecificCV(cloneDeep(specificCVs[ind]));
        setMode("SPECIFIC_CV");
    };

    const onShowMasterForm = () => {
        setShowLoading(false);
        setMode("MASTER_CV");
    };

    const onShowDashboard = () => {
        setMode(currentMode === "DASHBOARD" ? prevMode : "DASHBOARD");
    };

    const getRemovedItems = (cvDefault, cv) => {
        const educationIds =
            differenceBy(
                cvDefault ? cvDefault.education : [],
                cv.education,
                "id")
                .map(e => e.id ? parseInt(e.id, 10) : 0);
        const certificationIds =
            differenceBy(
                cvDefault ? cvDefault.certification : [],
                cv.certification,
                "id")
                .map(e => e.id ? parseInt(e.id, 10) : 0);
        const experienceIds =
            differenceBy(
                cvDefault ? cvDefault.experience : [],
                cv.experience,
                "id")
                .map(e => e.id ? parseInt(e.id, 10) : 0);
        const skillIds =
            differenceBy(
                (cvDefault && cvDefault.skills) ? cvDefault.skills.hardSkills : [],
                cv.skills.hardSkills,
                "id")
                .map(e => parseInt(e.id, 10));
        if (
            !cv.id ||
            (
                educationIds.length === 0 &&
                certificationIds.length === 0 &&
                experienceIds.length === 0 &&
                skillIds.length === 0
            )) {
            return null;
        }
        const removedItems = {
            cvId: cv.id,
            certificationIds,
            educationIds,
            experienceIds,
            skillIds,
        };
        return removedItems;
    };

    const saveCV = (cv) => {
        saveMyCV(cv)
            .then((res) => {
                if (res) {
                    const cvRes = formatAsCVData(res);
                    setShowSaving(false);
                    if (currentMode === "MASTER_CV") {
                        setMasterCV(cvRes);
                        setMasterCVDefault(cloneDeep(cvRes));
                    } else {
                        const newSpecificCvs = [...specificCVs];
                        newSpecificCvs[indexSCV] = cvRes;
                        setSpecificCVs(newSpecificCvs);
                    }
                }
            });
    };

    const removeCVItem = async (removedItems) => {
        return removeCVFields(removedItems);
    };

    const saveChanges = () => {
        const cv = currentMode === "MASTER_CV" ? masterCV : specificCV;
        const compareToCv = currentMode === "MASTER_CV" ? masterCVDefault : specificCVs[indexSCV];
        if (cv) {
            schema
                .validate(cv, { abortEarly: false, strict: true, context: { cv } })
                .then(() => {
                    const formattedCv = formatAsCVType(cv);
                    setShowSaving(true);

                    const removedItems = getRemovedItems(compareToCv, cv);
                    if (removedItems) {
                        removeCVItem(removedItems).then(() => saveCV(formattedCv));
                    } else {
                        saveCV(formattedCv);
                    }
                    setValidateOnChange(false);
                })
                .catch((err) => {
                    setValidateOnChange(true);
                    setFormErrors(err);
                });
        }
    };

    const onShowPdf = async (cvId) => {
        const resHtmlCv = await getHtmlCv(cvId);
        setHtmlCv({ cvId, html: resHtmlCv });
    };

    const onDeleteCv = async (id, arrId) => {
        const newSpecificCvs = [...specificCVs];
        newSpecificCvs.splice(arrId, 1);
        setSpecificCVs(newSpecificCvs);
        await deleteCv(id);
    };

    return (
        <>
            <Head>
                <title>JobTailor - Tailor</title>
            </Head>
            <AuthCheckBanner>
                {
                    !showLoading && currentMode !== "NO_CV" && //indexSCV === -1 &&
                    <DashSelector
                        cv={currentMode === "MASTER_CV" ? masterCV : specificCV}
                        mode={currentMode}
                        onShowDashboard={onShowDashboard}
                        onChange={(event) => onFieldChange({
                            event,
                            cv: currentMode === "MASTER_CV" ? masterCV : specificCV,
                            setCV: currentMode === "MASTER_CV" ? setMasterCV : setSpecificCV,
                        })}
                    />
                }
                {
                    !showLoading && currentMode === "DASHBOARD" && masterCV &&
                    <Dashboard
                        masterCV={masterCV}
                        specificCVs={specificCVs}
                        indexSCV={indexSCV}
                        onCloneCV={() => onCloneCV({
                            cv: masterCV,
                            specificCVs,
                            setCV: setSpecificCVs,
                        })}
                        onCreateCV={() => onCreateCV({
                            cv: masterCV,
                            specificCVs,
                            setCV: setSpecificCVs,
                        })}
                        onEditCV={onEditCV}
                        onShowPdf={onShowPdf}
                        htmlCv={htmlCv}
                        onDeleteCv={onDeleteCv}
                    />
                }
                {
                    !showLoading && currentMode === "MASTER_CV" && masterCV &&
                    <CV
                        cvID="master"
                        cv={masterCV}
                        onChange={(event) => onFieldChange({ event, cv: masterCV, setCV: setMasterCV })}
                        onSkillAction={(event) =>  onSkillAction({ event, cv: masterCV, setCV: setMasterCV })}
                        onItemAction={(event) => onItemAction({ event, cv: masterCV, setCV: setMasterCV })}
                        onItemMove={(event) => onItemMove({ event, cv: masterCV, setCV: setMasterCV })}
                        cvValidation={cvValidation}
                    />
                }
                {
                    !showLoading && currentMode === "SPECIFIC_CV" && specificCV &&
                    <CV
                        cvID="second"
                        cv={specificCV}
                        onChange={(event) =>
                            onFieldChange({ event, cv: specificCV, setCV: setSpecificCV })}
                        onSkillAction={(event) =>
                            onSkillAction({ event, cv: specificCV, setCV: setSpecificCV })}
                        onItemAction={(event) =>
                            onItemAction({ event, cv: specificCV, setCV: setSpecificCV })}
                        onItemMove={(event) =>
                            onItemMove({ event, cv: specificCV, setCV: setSpecificCV })}
                        cvValidation={cvValidation}
                    />
                }
                {
                    !showLoading && currentMode === "NO_CV" &&
                    <NoMasterCV onButtonClick={onShowMasterForm} />
                }
                {
                    showSaving &&
                    <div className="m-2 p-0 fixed-bottom text-success"><i className="fa fa-cloud-upload" /></div>
                }
                {
                    showLoading &&
                    <div className="m-2 p-0 fixed-bottom text-success"><i className="fa fa-cloud-download" /></div>
                }
                {
                    !showLoading && (currentMode === "MASTER_CV" || currentMode === "SPECIFIC_CV") &&
                    <Row className="m-0 pt-2 pb-2 text-right" style={{ borderTop: "1px solid #dee2e6" }}>
                        <Col className="pr-2 pl-2 mb-3">
                            <Button
                                variant="primary"
                                disabled={showSaving || (validateOnChange ? !cvValidation.formState.isValid : false)}
                                onClick={saveChanges}
                            >
                                Save
                            </Button>
                        </Col>
                    </Row>
                }
            </AuthCheckBanner>
        </>
    );
};

export default Tailor;
